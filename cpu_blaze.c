#define _GNU_SOURCE
#include <stdio.h> // para entrada/saída (ex.: printf)
#include <stdlib.h> // para malloc, atoi, atof
#include <string.h> // para manipular strings (strcmp, strtok)
#include <time.h> // para nanosleep e clock_gettime

#ifdef _WIN32
    #include <windows.h> // para threads e manipulação de eventos
    #include <process.h> // para CreateThread
    #include <io.h>
    typedef HANDLE thread_t;
    typedef DWORD thread_return_t;
    volatile int running = 1;
    
    BOOL WINAPI handle_ctrl_c(DWORD ctrl_type) {
        if (ctrl_type == CTRL_C_EVENT) {
            running = 0;
            return TRUE;
        }
        return FALSE;
    }
#else
    #include <signal.h>
    #include <pthread.h>
    #include <unistd.h>
    #include <time.h>
    typedef pthread_t thread_t;
    typedef void* thread_return_t;
    volatile sig_atomic_t running = 1;
    
    void handle_sigint(int sig) {
        running = 0;
    }
#endif

typedef struct {
    int core_id;
    int percent;
    int mode_multi;
    int* core_ids;
    int num_cores;
    int rotation_enabled;
    int rotation_seconds;
} ThreadArg;

#ifdef _WIN32
typedef struct {
    HANDLE* threads;
    int* core_ids;
    int num_threads;
    int num_cores;
    int rotation_seconds;
} RotationArg;
#else
typedef struct {
    pthread_t* threads;
    int* core_ids;
    int num_threads;
    int num_cores;
    int rotation_seconds;
} RotationArg;
#endif

#ifdef _WIN32
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
#else
    void* burn_thread(void* arg) {
        ThreadArg* t_arg = (ThreadArg*)arg;
        int percent = t_arg->percent;
        unsigned long long result = 0;
        
        while (running) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            double cycle_ms = 100.0;
            double work_ms = (percent / 100.0) * cycle_ms;
            double elapsed_ms = 0.0;
            
            while (running && elapsed_ms < work_ms) {
#if defined(__x86_64__)
                __asm__ volatile (
                    "movq $1, %%rax\n"
                    "imulq $2, %%rax\n"
                    "addq %%rax, %0"
                    : "+r" (result)
                    :
                    : "rax"
                );
#elif defined(__aarch64__)
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
                result += 1 * 2;
#endif
                clock_gettime(CLOCK_MONOTONIC, &end);
                elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
                            (end.tv_nsec - start.tv_nsec) / 1000000.0;
            }
            
            double sleep_ms = cycle_ms - elapsed_ms;
            if (sleep_ms > 0) {
                struct timespec ts = {0, (long)(sleep_ms * 1000000)};
                nanosleep(&ts, NULL);
            }
        }
        return NULL;
    }
#endif

#ifdef _WIN32
    DWORD WINAPI timer_thread(LPVOID arg) {
        double time_min = *(double*)arg;
        DWORD sleep_ms = (DWORD)(time_min * 60 * 1000);
        Sleep(sleep_ms);
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
#else
    void* timer_thread(void* arg) {
        double time_min = *(double*)arg;
        long seconds = (long)(time_min * 60);
        long nanos = (long)((time_min * 60 - seconds) * 1000000000);
        struct timespec ts = {seconds, nanos};
        nanosleep(&ts, NULL);
        running = 0;
        return NULL;
    }

    void* rotation_thread(void* arg) {
        RotationArg* rot_arg = (RotationArg*)arg;
        int current_core_index = 0;
        
        while (running) {
            struct timespec ts = {rot_arg->rotation_seconds, 0};
            nanosleep(&ts, NULL);
            if (!running) break;
            
            // Rotaciona para o próximo núcleo
            int target_core = rot_arg->core_ids[current_core_index];
            
            for (int i = 0; i < rot_arg->num_threads; i++) {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(target_core, &cpuset);
                pthread_setaffinity_np(rot_arg->threads[i], sizeof(cpuset), &cpuset);
            }
            
            current_core_index = (current_core_index + 1) % rot_arg->num_cores;
        }
        return NULL;
    }
#endif

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleCtrlHandler(handle_ctrl_c, TRUE);
#else
    signal(SIGINT, handle_sigint);
#endif

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
        } else if (strcmp(argv[i], "-r") == 0) {
            rotation_seconds = atoi(argv[i + 1]);
            if (rotation_seconds < 0) rotation_seconds = 0;
        }
    }

#ifdef _WIN32
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int max_cores = sysInfo.dwNumberOfProcessors;
#else
    int max_cores = sysconf(_SC_NPROCESSORS_ONLN);
#endif

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

#ifndef _WIN32
    if (!mode_multi) {
        printf("Nota: No macOS, a afinidade de núcleos não é garantida. O sistema distribuirá as threads.\n");
    }
#endif

    printf("Iniciando CPU burn: %d núcleos, %.2f minutos, %d%% uso, modo %s", 
           num_cores, time_min, percent, mode_multi ? "multi" : "single");
    if (rotation_seconds > 0) {
        printf(", rotação a cada %d segundos", rotation_seconds);
    }
    printf(". Pressione Ctrl+C para parar.\n");

    int num_threads = num_cores;
#ifdef _WIN32
    HANDLE* threads = malloc(sizeof(HANDLE) * num_threads);
#else
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
#endif
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
#ifdef _WIN32
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
#else
        if (pthread_create(&threads[i], NULL, burn_thread, &args[i]) != 0) {
            printf("Erro ao criar thread %d\n", i);
        }
        if (rotation_seconds > 0) {
            // Com rotação, definir afinidade inicial para o primeiro núcleo
            // A thread de rotação vai mudar depois
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cores[0], &cpuset);
            pthread_setaffinity_np(threads[i], sizeof(cpuset), &cpuset);
        } else if (!mode_multi) {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(cores[i], &cpuset);
            pthread_setaffinity_np(threads[i], sizeof(cpuset), &cpuset);
        } else {
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            for (int j = 0; j < num_cores; j++) {
                CPU_SET(cores[j], &cpuset);
            }
            pthread_setaffinity_np(threads[i], sizeof(cpuset), &cpuset);
        }
#endif
    }

#ifdef _WIN32
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
#else
    pthread_t timer;
    pthread_create(&timer, NULL, timer_thread, &time_min);
    
    pthread_t rotation;
    int rotation_created = 0;
    
    // Criar thread de rotação se necessário
    if (rotation_seconds > 0 && num_cores > 1) {
        RotationArg* rot_arg = malloc(sizeof(RotationArg));
        rot_arg->threads = threads;
        rot_arg->core_ids = cores;
        rot_arg->num_threads = num_threads;
        rot_arg->num_cores = num_cores;
        rot_arg->rotation_seconds = rotation_seconds;
        if (pthread_create(&rotation, NULL, rotation_thread, rot_arg) == 0) {
            rotation_created = 1;
        }
    }
    
    void* retval;
    while (running) {
        int finished = 1;
        for (int i = 0; i < num_threads; i++) {
            if (pthread_tryjoin_np(threads[i], &retval) != 0) {
                finished = 0;
                break;
            }
        }
        if (finished) break;
        usleep(100000);
    }
    
    for (int i = 0; i < num_threads; i++) {
        if (!running) pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }
    if (timer) pthread_cancel(timer);
    pthread_join(timer, NULL);
    if (rotation_created) {
        pthread_cancel(rotation);
        pthread_join(rotation, NULL);
    }
#endif

    free(threads);
    free(args);
    free(cores);

    printf("CPU burn encerrado.\n");
    return 0;
}