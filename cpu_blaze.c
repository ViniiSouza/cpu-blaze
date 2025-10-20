#include <stdio.h>      // Para entrada/saída (ex.: printf)
#include <stdlib.h>     // Para malloc, atoi, atof
#include <string.h>     // Para manipular strings (strcmp, strtok)
#include <signal.h>     // Para capturar Ctrl+C (SIGINT)
#include <pthread.h>    // Para threads POSIX
#include <unistd.h>     // Para sysconf (número de núcleos) e sleep
#include <time.h>       // Para nanosleep e clock_gettime

// Variável global para controlar o loop de estresse
// 'volatile' evita otimizações, 'sig_atomic_t' é seguro para sinais
volatile sig_atomic_t running = 1;

// Função que lida com Ctrl+C
void handle_sigint(int sig) {
    running = 0;
}

// Estrutura para argumentos da thread
typedef struct {
    int core_id;    // ID do núcleo (usado para identificação no modo single)
    int percent;    // Porcentagem de uso da CPU
    int mode_multi; // 0 = single (uma thread por núcleo), 1 = multi (uma thread em múltiplos núcleos)
    int* core_ids;  // Array de IDs dos núcleos (modo multi)
    int num_cores;  // Número de núcleos (modo multi)
} ThreadArg;

// Função de estresse da CPU
void* burn_thread(void* arg) {
    ThreadArg* t_arg = (ThreadArg*)arg;
    int percent = t_arg->percent;

    // No macOS, não definimos afinidade explícita (nem no modo single nem multi)
    // O scheduler do macOS gerencia a distribuição das threads entre os núcleos
    // No modo single, criamos uma thread por núcleo; no modo multi, uma thread para todos

    unsigned long long result = 0;
    while (running) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start); // Marca o tempo inicial

        double cycle_ms = 100.0; // Ciclo de 100ms
        double work_ms = (percent / 100.0) * cycle_ms; // Tempo de trabalho
        double elapsed_ms = 0.0;

        // Loop interno: estressa até atingir work_ms
        while (running && elapsed_ms < work_ms) {
#if defined(__x86_64__)
            // x86_64 (Mac Intel)
            __asm__ volatile (
                "movq $1, %%rax\n"
                "imulq $2, %%rax\n"
                "addq %%rax, %0"
                : "+r" (result)
                :
                : "rax"
            );
#elif defined(__aarch64__)
            // ARM64 (Apple Silicon)
            __asm__ volatile (
                "mov x0, #1\n"
                "mov x1, #2\n"
                "mul x0, x0, x1\n"
                "add %0, %0, x0"
                : "+r" (result)
                :
                : "x0", "x1"
            );
#else
            result += 1 * 2; // Fallback
#endif
            clock_gettime(CLOCK_MONOTONIC, &end);
            elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                        (end.tv_nsec - start.tv_nsec) / 1000000.0;
        }

        // Sleep pelo resto do ciclo
        double sleep_ms = cycle_ms - elapsed_ms;
        if (sleep_ms > 0) {
            struct timespec ts = {0, (long)(sleep_ms * 1000000)};
            nanosleep(&ts, NULL);
        }
    }
    return NULL;
}

// Função para o timer
void* timer_thread(void* arg) {
    double time_min = *(double*)arg;
    long seconds = (long)(time_min * 60);
    long nanos = (long)((time_min * 60 - seconds) * 1000000000);
    struct timespec ts = {seconds, nanos};
    nanosleep(&ts, NULL);
    running = 0;
    return NULL;
}

int main(int argc, char* argv[]) {
    signal(SIGINT, handle_sigint);

    // Valores padrão
    int* cores = NULL;
    int num_cores = 0;
    double time_min = 1.0;
    int percent = 100;
    int mode_multi = 0;

    // Parsear argumentos
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            printf("Erro: argumento %s precisa de valor\n", argv[i]);
            return 1;
        }
        if (strcmp(argv[i], "-c") == 0) {
            char* token = strtok(argv[i + 1], ",");
            cores = malloc(sizeof(int) * 64);
            while (token) {
                int core = atoi(token);
                if (core >= 0) cores[num_cores++] = core;
                token = strtok(NULL, ",");
            }
        } else if (strcmp(argv[i], "-t") == 0) {
            time_min = atof(argv[i + 1]);
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

    // Default: todos os núcleos
    if (num_cores == 0) {
        num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        cores = malloc(sizeof(int) * num_cores);
        for (int i = 0; i < num_cores; i++) cores[i] = i;
    }

    // Validar núcleos
    int max_cores = sysconf(_SC_NPROCESSORS_ONLN);
    for (int i = 0; i < num_cores; i++) {
        if (cores[i] >= max_cores) {
            printf("Erro: núcleo %d inválido (máximo: %d)\n", cores[i], max_cores - 1);
            free(cores);
            return 1;
        }
    }

    // Aviso sobre afinidade no macOS
    if (!mode_multi) {
        printf("Nota: No macOS, a afinidade de núcleos não é garantida. O sistema distribuirá as threads.\n");
    }

    printf("Iniciando CPU burn: %d núcleos, %.2f minutos, %d%% uso, modo %s. Pressione Ctrl+C para parar.\n",
           num_cores, time_min, percent, mode_multi ? "multi" : "single");

    // Criar threads
    int num_threads = mode_multi ? 1 : num_cores;
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
    ThreadArg* args = malloc(sizeof(ThreadArg) * num_threads);

    for (int i = 0; i < num_threads; i++) {
        args[i].core_id = mode_multi ? 0 : cores[i];
        args[i].percent = percent;
        args[i].mode_multi = mode_multi;
        args[i].core_ids = cores;
        args[i].num_cores = num_cores;
        if (pthread_create(&threads[i], NULL, burn_thread, &args[i]) != 0) {
            printf("Erro ao criar thread %d\n", i);
        }
    }

    // Thread timer
    pthread_t timer;
    pthread_create(&timer, NULL, timer_thread, &time_min);

    // Aguardar threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(timer, NULL);

    // Liberar recursos
    free(threads);
    free(args);
    free(cores);

    printf("CPU burn encerrado.\n");
    return 0;
}