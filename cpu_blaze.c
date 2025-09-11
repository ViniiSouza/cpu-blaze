#include <stdio.h>    // Para entrada/saída (ex.: printf para mostrar mensagens no console)
#include <stdlib.h>   // Para funções como malloc (alocar memória), atoi (string para inteiro), atof (string para double)
#include <string.h>   // Para manipular strings (ex.: strcmp para comparar, strtok para dividir)
#include <signal.h>   // Para capturar Ctrl+C (sinal SIGINT)
#include <windows.h>  // Biblioteca do Windows para threads, afinidade e sleep

// Variável global para controlar se o programa deve continuar rodando
volatile sig_atomic_t running = 1;

// Função que lida com Ctrl+C
void handle_sigint(int sig) {
    running = 0;
}

typedef struct {
    int core_id;    // ID do núcleo
    int percent;    // Porcentagem de uso da CPU
    int mode_multi; // 0 = single (uma thread por núcleo), 1 = multi (uma thread em múltiplos núcleos)
    int* core_ids;  // Array de IDs dos núcleos (usado apenas no modo multi)
    int num_cores;  // Número de núcleos (usado no modo multi)
} ThreadArg;

// Função que estressa a CPU (executada por cada thread)
DWORD WINAPI blaze_thread(LPVOID arg) {
    // Converte o argumento genérico (void*) para nossa estrutura
    ThreadArg* t_arg = (ThreadArg*)arg;
    int core_id = t_arg->core_id;
    int percent = t_arg->percent;
    int mode_multi = t_arg->mode_multi;
    int* core_ids = t_arg->core_ids;
    int num_cores = t_arg->num_cores;

    // Configurar afinidade (especifica em qual núcleo a thread pode rodar)
    DWORD_PTR affinity_mask = 0;
    if (mode_multi) {
        // Modo multi: combinar todos os núcleos na máscara
        // Ex.: para núcleos 0 e 1, affinity_mask = 00000011 (em binário)
        for (int i = 0; i < num_cores; i++) {
            affinity_mask |= 1ULL << core_ids[i]; // Operador '|' junta os bits
        }
    } else {
        // Modo single: usar apenas o núcleo especificado
        affinity_mask = 1ULL << core_id; // Ex.: núcleo 0 = 00000001
    }
    // Aplica a máscara à thread atual
    if (!SetThreadAffinityMask(GetCurrentThread(), affinity_mask)) {
        printf("Erro ao definir afinidade para núcleo %d\n", core_id);
        return 1;
    }

    // Variável para acumular cálculos (evita otimizações do compilador)
    unsigned long long result = 0;
    // Obtém a frequência do contador de alta precisão (para medir tempo)
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    // Loop principal de estresse
    while (running) {
        LARGE_INTEGER start, end;
        QueryPerformanceCounter(&start); // Marca o tempo inicial

        // Ciclo de 100ms para controlar a porcentagem
        double cycle_ms = 100.0; // Duração do ciclo (0.1s)
        double work_ms = (percent / 100.0) * cycle_ms; // Tempo de trabalho (ex.: 50ms para 50%)
        double elapsed_ms = 0.0; // Tempo gasto no trabalho

        // Loop interno: faz cálculos até atingir work_ms
        while (running && elapsed_ms < work_ms) {
            // Assembly inline para estressar a CPU
#if defined(_M_X64) || defined(__x86_64__)
            // x86_64: usa registrador rax (64 bits)
            __asm__ volatile (
                "movq $1, %%rax\n"      // Coloca 1 em rax
                "imulq $2, %%rax\n"     // Multiplica rax por 2
                "addq %%rax, %0"        // Adiciona rax à variável result
                : "+r" (result)         // Output: result é modificado
                :                       // Input: nenhum
                : "rax"                 // Clobbers: rax é usado
            );
#elif defined(_M_ARM64) || defined(__aarch64__)
            // ARM64: usa registradores x0, x1
            __asm__ volatile (
                "mov x0, #1\n"          // Coloca 1 em x0
                "mov x1, #2\n"          // Coloca 2 em x1
                "mul x0, x0, x1\n"      // Multiplica x0 por x1
                "add %0, %0, x0"        // Adiciona x0 a result
                : "+r" (result)
                :
                : "x0", "x1"
            );
#else
            // Fallback em C puro se a arquitetura não for suportada
            result += 1 * 2;
#endif
            QueryPerformanceCounter(&end); // Marca o tempo final
            // Calcula tempo decorrido em milissegundos
            elapsed_ms = ((end.QuadPart - start.QuadPart) * 1000.0) / freq.QuadPart;
        }

        // Sleep pelo resto do ciclo (ex.: 50ms para 50% em 100ms)
        double sleep_ms = cycle_ms - elapsed_ms;
        if (sleep_ms > 0) {
            Sleep((DWORD)sleep_ms);
        }
    }
    return 0;
}

// Função para o timer (para o programa após o tempo especificado)
DWORD WINAPI timer_thread(LPVOID arg) {
    double time_min = *(double*)arg; // Tempo em minutos
    Sleep((DWORD)(time_min * 60 * 1000)); // Converte para milissegundos
    running = 0;
    return 0;
}

int main(int argc, char* argv[]) {
    // Configura o manipulador de Ctrl+C
    signal(SIGINT, handle_sigint);

    // Valores padrão
    int* cores = NULL;       // Array para IDs dos núcleos
    int num_cores = 0;       // Número de núcleos selecionados
    double time_min = 1.0;   // Tempo em minutos (suporta decimais)
    int percent = 100;       // Porcentagem padrão (100%)
    int mode_multi = 0;      // 0 = single, 1 = multi

    // Parsear argumentos da linha de comando
    // Ex.: "cpu_blaze.exe -c 0,1 -t 0.5 -p 80 -m single"
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            printf("Erro: argumento %s precisa de valor\n", argv[i]);
            return 1;
        }
        if (strcmp(argv[i], "-c") == 0) {
            char* token = strtok(argv[i + 1], ","); // Divide "0,1" em "0", "1"
            cores = malloc(sizeof(int) * 64); // Aloca memória para até 64 núcleos
            while (token) {
                int core = atoi(token); // Converte string para inteiro
                if (core >= 0) cores[num_cores++] = core;
                token = strtok(NULL, ",");
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            time_min = atof(argv[i + 1]); // Converte para double (ex.: 0.5)
            if (time_min <= 0) time_min = 1.0;
        } else if (strcmp(argv[i], "-p") == 0) {
            percent = atoi(argv[i + 1]);
            if (percent < 0 || percent > 100) percent = 100;
        } else if (strcmp(argv[i], "-m") == 0) {
            if (strcmp(argv[i + 1], "multi") == 0) mode_multi = 1;
            else if (strcmp(argv[i + 1], "single") == 0) mode_multi = 0;
            else {
                printf("Erro: modo inválido (%s). Use 'single' ou 'multi'.\n", argv[i + 1]);
                return 1;
            }
        }
    }

    // Se nenhum núcleo for especificado, usar todos
    if (num_cores == 0) {
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo); // Obtém número de núcleos
        num_cores = sysinfo.dwNumberOfProcessors;
        cores = malloc(sizeof(int) * num_cores);
        for (int i = 0; i < num_cores; i++) cores[i] = i;
    }

    // Validar núcleos
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    for (int i = 0; i < num_cores; i++) {
        if (cores[i] >= (int)sysinfo.dwNumberOfProcessors) {
            printf("Erro: núcleo %d inválido (máximo: %d)\n", cores[i], sysinfo.dwNumberOfProcessors - 1);
            free(cores);
            return 1;
        }
    }

    // Exibir configuração
    printf("Iniciando CPU blaze: %d núcleos, %.2f minutos, %d%% uso, modo %s. Pressione Ctrl+C para parar.\n",
           num_cores, time_min, percent, mode_multi ? "multi" : "single");

    // Criar threads
    int num_threads = mode_multi ? 1 : num_cores; // Uma thread para multi, uma por núcleo para single
    HANDLE* threads = malloc(sizeof(HANDLE) * num_threads);
    ThreadArg* args = malloc(sizeof(ThreadArg) * num_threads);

    for (int i = 0; i < num_threads; i++) {
        args[i].core_id = mode_multi ? 0 : cores[i]; // No modo single, cada thread usa um núcleo
        args[i].percent = percent;
        args[i].mode_multi = mode_multi;
        args[i].core_ids = cores; // Para modo multi
        args[i].num_cores = num_cores;
        threads[i] = CreateThread(NULL, 0, blaze_thread, &args[i], 0, NULL);
        if (threads[i] == NULL) {
            printf("Erro ao criar thread %d\n", i);
        }
    }

    // Thread timer
    HANDLE timer = CreateThread(NULL, 0, timer_thread, &time_min, 0, NULL);

    // Aguardar threads
    WaitForMultipleObjects(num_threads, threads, TRUE, INFINITE);
    WaitForSingleObject(timer, INFINITE);

    // Liberar recursos
    for (int i = 0; i < num_threads; i++) CloseHandle(threads[i]);
    CloseHandle(timer);
    free(threads);
    free(args);
    free(cores);

    printf("CPU blaze encerrado.\n");
    return 0;
}