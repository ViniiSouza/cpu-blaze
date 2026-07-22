# CPU Blaze

A controlled CPU stress-testing tool built for an Operating Systems course: a **C engine** that generates precise CPU load, driven by a **C# (.NET 8) Windows Forms interface** that manages processes, cores and monitoring.

## What it demonstrates

- **Process management**: the C# interface spawns and controls the native C stress process.
- **CPU affinity**: pin load to specific cores (all, odd, even, or a custom set like `0,2,4`).
- **Threads and concurrency**: per-core worker threads generating load at a target percentage.
- **C/C# interop**: the .NET build compiles the C module automatically when GCC is available.

## Features

- **Core selection**: all cores, odd, even, or custom list.
- **Load modes**: *Single* (each selected core runs at the target %) or *Multi* (the target % is split across the selected cores).
- **Configurable duration** and load percentage (1–100%).
- **Core rotation**: alternates the stress between cores at a configurable interval.
- **Live monitoring**: real-time status panel and operation log; automatic core-count detection.

## Requirements

- Windows 10/11 (64-bit)
- .NET 8 SDK
- GCC (MinGW-w64 or MSYS2) on PATH, used to compile the C engine

## Build and run

```bash
# 1. Compile the C engine (skipped if the C# build finds GCC and does it automatically)
gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall

# 2. Build and run the interface
cd CpuBlazeInterface/CpuBlazeInterface
dotnet run
```

Or open `CpuBlazeInterface.slnx` in Visual Studio 2022 and press F5. The interface expects `cpu_blaze_windows.exe` in the project root or next to the executable.

### Example: distributed load

Select cores `0,1,2` in *Multi* mode at 100% → each core receives ≈33% load for the configured duration.

## Project structure

```
cpu-blaze/
├── cpu_blaze.c                 # C engine: load generation with affinity control
└── CpuBlazeInterface/          # C# .NET 8 Windows Forms UI
    ├── Form1.cs                # Main interface
    ├── CpuBlazeRunner.cs       # Process manager
    └── CpuBlazeCompiler.cs     # Auto-compilation of the C module
```

> ⚠️ This tool intentionally generates high CPU load. Monitor system temperature and close sensitive applications before running intensive tests.

## Academic context

Developed for the Operating Systems course at FURB (Computer Science), demonstrating process management, thread scheduling, CPU affinity and inter-process communication.
