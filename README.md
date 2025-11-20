# CPU Blaze - CPU Stress Tester

Ferramenta simples para estressar a CPU com controle de porcentagem, núcleos e tempo de execução.

## Compilação

### Compilação Manual

#### Windows
```bash
gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall
```

Ou use os scripts fornecidos:
- **Batch**: `build_cpu_blaze.bat`
- **PowerShell**: `build_cpu_blaze.ps1`

#### Linux/MacOS
```bash
gcc cpu_blaze.c -o cpu_blaze -lpthread -Wall
```

Ou use o script fornecido:
- **Bash**: `./build_cpu_blaze.sh`

### Compilação Automática

A interface Windows Forms (`CpuBlazeInterface`) compila automaticamente o código C durante o build do projeto, se o GCC estiver disponível no PATH.

Você também pode compilar manualmente usando o botão "Compilar C" na interface.

### Requisitos

- **GCC** (GNU Compiler Collection)
  - Windows: Instale [MinGW-w64](https://www.mingw-w64.org/) ou [MSYS2](https://www.msys2.org/)
  - Linux: `sudo apt-get install gcc` (Ubuntu/Debian) ou equivalente
  - macOS: Já incluído no Xcode Command Line Tools (`xcode-select --install`)

## Uso

### Interface Gráfica (Windows Forms)

Execute a aplicação `CpuBlazeInterface.exe` para usar a interface gráfica com todas as funcionalidades:
- Detecção automática de núcleos
- Seleção de núcleos (todos, ímpares, pares, personalizado)
- Modo single/multi
- Controle de porcentagem e tempo
- Rotação de núcleos
- Compilação do código C integrada

### Linha de Comando

```bash
./cpu_blaze_windows.exe [parâmetros]
```

## Parâmetros

| Parâmetro | Descrição | Padrão |
|-----------|-----------|--------|
| `-c N` | Núcleos específicos (separados por vírgula) | Todos os núcleos |
| `-t MIN` | Tempo de execução em minutos | 1.0 minuto |
| `-p PERCENT` | Porcentagem de uso da CPU (0-100) | 100% |
| `-m MODE` | Modo: `single` (100% por núcleo) ou `multi` (distribuído) | single |
| `-r SECONDS` | Rotação de núcleos (tempo em segundos por núcleo) | Desabilitado (0) |

## Exemplos

```bash
# Estressa todos os núcleos por 1 minuto a 100%
./cpu_blaze_windows.exe

# Estressa núcleos 0,1,2 por 2 minutos a 50%
./cpu_blaze_windows.exe -c 0,1,2 -t 2 -p 50

# Estressa núcleos 0,1 distribuindo 100% entre eles
./cpu_blaze_windows.exe -c 0,1 -p 100 -m multi

# Estressa por 30 segundos
./cpu_blaze_windows.exe -t 0.5

# Rotaciona entre núcleos a cada 10 segundos
./cpu_blaze_windows.exe -c 0,1,2,3 -r 10
```

## Notas

- Pressione `Ctrl+C` para parar a qualquer momento
- No modo `single`, cada núcleo usa a porcentagem especificada
- No modo `multi`, a porcentagem é distribuída entre os núcleos especificados
- Com rotação (`-r`), as threads alternam entre os núcleos selecionados a cada X segundos
