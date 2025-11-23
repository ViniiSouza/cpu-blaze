#include <stdio.h> // para entrada/saída (ex.: printf)
#include <stdlib.h> // para malloc, atoi, atof
#include <string.h> // para manipular strings (strcmp, strtok)
#include <windows.h> // para threads e manipulação de eventos
#include <process.h> // para CreateThread
#include <io.h>

// Função auxiliar para converter string para double, suportando tanto vírgula quanto ponto
double parse_double(const char* str) {
    char* buffer = malloc(strlen(str) + 1);
    strcpy(buffer, str);
    for (int i = 0; buffer[i]; i++) {
        if (buffer[i] == ',') {
            buffer[i] = '.';
        }
    }
    double result = atof(buffer);
    free(buffer);
    return result;
}

volatile int running = 1;

BOOL WINAPI handle_ctrl_c(DWORD ctrl_type) {
    if (ctrl_type == CTRL_C_EVENT) {
        running = 0;
        return TRUE;
    }
    return FALSE;
}

typedef struct {
    int core_id;
    int percent;
    int mode_multi;
    int* core_ids;
    int num_cores;
    int rotation_enabled;
    int rotation_seconds;
} ThreadArg;

typedef struct {
    HANDLE* threads;
    int* core_ids;
    int num_threads;
    int num_cores;
    int rotation_seconds;
} RotationArg;

DWORD WINAPI burn_thread(LPVOID arg) {
    ThreadArg* t_arg = (ThreadArg*)arg;
    int percent = t_arg->percent;
    unsigned long long result = 0;
    
    while (running) {
        LARGE_INTEGER freq, start, end;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);
        
        double cycle_ms = 100.0;
        double work_ms = (percent / 100.0) * cycle_ms;
        double elapsed_ms = 0.0;
        
        while (running && elapsed_ms < work_ms) {
            for (int i = 0; i < 1000; i++) {
                result += 1 * 2;
            }
            QueryPerformanceCounter(&end);
            elapsed_ms = ((double)(end.QuadPart - start.QuadPart) / freq.QuadPart) * 1000.0;
        }
        
        double sleep_ms = cycle_ms - elapsed_ms;
        if (sleep_ms > 0) {
            Sleep((DWORD)sleep_ms);
        }
    }
    return 0;
}

DWORD WINAPI timer_thread(LPVOID arg) {
    double time_min = *(double*)arg;
    double total_ms_double = time_min * 60.0 * 1000.0;
    DWORD total_ms = (DWORD)total_ms_double;
    
    if (total_ms == 0 && total_ms_double > 0.5) {
        // Se arredondou para 0 mas deveria ser maior, usar pelo menos 1ms
        total_ms = 1;
    }
    
    Sleep(total_ms);
    running = 0;
    return 0;
}

DWORD WINAPI rotation_thread(LPVOID arg) {
    RotationArg* rot_arg = (RotationArg*)arg;
    int current_core_index = 0;
    
    while (running) {
        Sleep(rot_arg->rotation_seconds * 1000);
        if (!running) break;
        
        // Rotaciona para o próximo núcleo
        int target_core = rot_arg->core_ids[current_core_index];
        
        for (int i = 0; i < rot_arg->num_threads; i++) {
            if (rot_arg->threads[i]) {
                DWORD_PTR mask = (DWORD_PTR)(1ULL << target_core);
                SetThreadAffinityMask(rot_arg->threads[i], mask);
            }
        }
        
        current_core_index = (current_core_index + 1) % rot_arg->num_cores;
    }
    return 0;
}

int main(int argc, char* argv[]) {
    SetConsoleCtrlHandler(handle_ctrl_c, TRUE);

    int* cores = NULL;
    int num_cores = 0;
    double time_min = 1.0;
    int percent = 100;
    int mode_multi = 0;
    int rotation_seconds = 0;

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
            time_min = parse_double(argv[i + 1]);
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
        } else if (strcmp(argv[i], "-r") == 0) {
            rotation_seconds = atoi(argv[i + 1]);
            if (rotation_seconds < 0) rotation_seconds = 0;
        }
    }

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int max_cores = sysInfo.dwNumberOfProcessors;

    if (num_cores == 0) {
        num_cores = max_cores;
        cores = malloc(sizeof(int) * num_cores);
        for (int i = 0; i < num_cores; i++) cores[i] = i;
    }

    for (int i = 0; i < num_cores; i++) {
        if (cores[i] >= max_cores) {
            printf("Erro: núcleo %d inválido (máximo: %d)\n", cores[i], max_cores - 1);
            free(cores);
            return 1;
        }
    }

    printf("Iniciando CPU burn: %d núcleos, %.2f minutos, %d%% uso, modo %s", 
           num_cores, time_min, percent, mode_multi ? "multi" : "single");
    if (rotation_seconds > 0) {
        printf(", rotação a cada %d segundos", rotation_seconds);
    }
    printf(". Pressione Ctrl+C para parar.\n");
    fflush(stdout); // Forçar a saída imediata da mensagem

    int num_threads = num_cores;
    HANDLE* threads = malloc(sizeof(HANDLE) * num_threads);
    ThreadArg* args = malloc(sizeof(ThreadArg) * num_threads);

    int thread_percent = mode_multi ? (percent / num_cores) : percent;

    for (int i = 0; i < num_threads; i++) {
        args[i].core_id = cores[i];
        args[i].percent = thread_percent;
        args[i].mode_multi = mode_multi;
        args[i].core_ids = cores;
        args[i].num_cores = num_cores;
        args[i].rotation_enabled = (rotation_seconds > 0) ? 1 : 0;
        args[i].rotation_seconds = rotation_seconds;
        
        threads[i] = CreateThread(NULL, 0, burn_thread, &args[i], 0, NULL);
        if (rotation_seconds > 0) {
            // Com rotação, definir afinidade inicial para o primeiro núcleo
            // A thread de rotação vai mudar depois
            DWORD_PTR mask = (DWORD_PTR)(1ULL << cores[0]);
            SetThreadAffinityMask(threads[i], mask);
        } else if (!mode_multi) {
            DWORD_PTR mask = (DWORD_PTR)(1ULL << cores[i]);
            SetThreadAffinityMask(threads[i], mask);
        } else {
            DWORD_PTR mask = 0;
            for (int j = 0; j < num_cores; j++) {
                mask |= (DWORD_PTR)(1ULL << cores[j]);
            }
            SetThreadAffinityMask(threads[i], mask);
        }
    }

    HANDLE timer = CreateThread(NULL, 0, timer_thread, &time_min, 0, NULL);
    HANDLE rotation = NULL;
    
    // Criar thread de rotação se necessário
    if (rotation_seconds > 0 && num_cores > 1) {
        RotationArg rot_arg;
        rot_arg.threads = threads;
        rot_arg.core_ids = cores;
        rot_arg.num_threads = num_threads;
        rot_arg.num_cores = num_cores;
        rot_arg.rotation_seconds = rotation_seconds;
        rotation = CreateThread(NULL, 0, rotation_thread, &rot_arg, 0, NULL);
    }
    
    HANDLE* all_handles = malloc(sizeof(HANDLE) * (num_threads + 1 + (rotation ? 1 : 0)));
    for (int i = 0; i < num_threads; i++) all_handles[i] = threads[i];
    all_handles[num_threads] = timer;
    if (rotation) all_handles[num_threads + 1] = rotation;
    
    while (running) {
        DWORD result = WaitForMultipleObjects(num_threads + 1 + (rotation ? 1 : 0), all_handles, FALSE, 100);
        if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + num_threads) {
            break;
        }
        if (!running) break;
    }
    
    for (int i = 0; i < num_threads; i++) {
        if (threads[i]) {
            TerminateThread(threads[i], 0);
            CloseHandle(threads[i]);
        }
    }
    if (timer) {
        TerminateThread(timer, 0);
        CloseHandle(timer);
    }
    if (rotation) {
        TerminateThread(rotation, 0);
        CloseHandle(rotation);
    }
    free(all_handles);

    free(threads);
    free(args);
    free(cores);

    printf("CPU burn encerrado.\n");
    return 0;
}
