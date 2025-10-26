# CPU Blaze - CPU Stress Tester

Ferramenta simples para estressar a CPU com controle de porcentagem, núcleos e tempo de execução.

## Compilação

### Windows
```bash
gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall
```

### Linux/MacOS
```bash
gcc cpu_blaze.c -o cpu_blaze -lpthread -Wall
```

## Uso

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
```

## Notas

- Pressione `Ctrl+C` para parar a qualquer momento
- No modo `single`, cada núcleo usa 100% da capacidade
- No modo `multi`, a porcentagem é distribuída entre os núcleos especificados
