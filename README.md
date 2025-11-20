# CPU Blaze - Ferramenta de Estresse de CPU

## 📋 Sobre o Projeto

O **CPU Blaze** é uma aplicação desenvolvida para a disciplina de Sistemas Operacionais, que permite realizar testes de estresse em processadores de forma controlada. A ferramenta oferece uma interface gráfica intuitiva desenvolvida em Windows Forms (C# .NET 8) para gerenciar e monitorar o uso de recursos do processador.

### Objetivos

- Permitir testes controlados de carga de CPU
- Demonstrar conceitos de gerenciamento de processos e threads
- Facilitar o estudo de afinidade de processadores (CPU affinity)
- Fornecer uma interface gráfica para análise de desempenho

## 🎯 Funcionalidades

A aplicação oferece as seguintes funcionalidades através de sua interface gráfica:

### Seleção de Núcleos
- **Todos os núcleos**: Utiliza todos os núcleos disponíveis no processador
- **Núcleos ímpares**: Utiliza apenas os núcleos com índice ímpar (1, 3, 5, ...)
- **Núcleos pares**: Utiliza apenas os núcleos com índice par (0, 2, 4, ...)
- **Personalizado**: Permite selecionar núcleos específicos manualmente

### Modos de Operação
- **Modo Single**: Cada núcleo selecionado utiliza a porcentagem especificada individualmente
- **Modo Multi**: A porcentagem total é distribuída entre todos os núcleos selecionados

### Controles de Estresse
- **Porcentagem de uso**: Controla o nível de carga (0-100%)
- **Tempo de execução**: Define a duração do teste em minutos
- **Rotação de núcleos**: Alterna o estresse entre núcleos a intervalos configuráveis

### Monitoramento
- Painel de status em tempo real
- Log de operações e mensagens do sistema
- Detecção automática do número de núcleos do processador

## 📦 Requisitos do Sistema

### Software
- **Windows 10/11** (64-bit)
- **.NET 8.0 Runtime** ou superior
- **Visual Studio 2022** (para desenvolvimento) ou **Visual Studio Build Tools**
- **GCC** (GNU Compiler Collection) - Opcional, para compilação do módulo C

### Hardware
- Processador com múltiplos núcleos (recomendado)
- Mínimo 4 GB de RAM
- Espaço em disco: ~50 MB

## 🚀 Instalação e Compilação

### Pré-requisitos

1. **Instalar .NET 8.0 SDK**
   - Baixe em: https://dotnet.microsoft.com/download/dotnet/8.0
   - Execute o instalador e siga as instruções

2. **Instalar Visual Studio 2022** (opcional, para desenvolvimento)
   - Inclua o workload "Desenvolvimento para desktop com .NET"
   - Inclua o componente "Windows Forms"

3. **Instalar GCC** (opcional, para compilação automática)
   - **MinGW-w64**: https://www.mingw-w64.org/
   - **MSYS2**: https://www.msys2.org/
   - Adicione o GCC ao PATH do sistema

### Compilação do Projeto

#### Método 1: Visual Studio (Recomendado)

1. Abra o arquivo `CpuBlazeInterface.slnx` no Visual Studio 2022
2. Aguarde a restauração dos pacotes NuGet
3. O projeto compilará automaticamente o módulo C (se o GCC estiver disponível)
4. Pressione `F5` para executar ou `Ctrl+Shift+B` para compilar

#### Método 2: Linha de Comando

```bash
# Navegue até o diretório do projeto
cd CpuBlazeInterface/CpuBlazeInterface

# Restaure as dependências
dotnet restore

# Compile o projeto
dotnet build

# Execute a aplicação
dotnet run
```

#### Método 3: Compilação do Executável

```bash
# Compile para produção
dotnet publish -c Release -r win-x64 --self-contained false

# O executável estará em:
# CpuBlazeInterface/CpuBlazeInterface/bin/Release/net8.0-windows/win-x64/publish/
```

### Compilação do Módulo C (Automática)

O módulo C é compilado automaticamente durante o build do projeto se:
- O GCC estiver disponível no PATH
- O arquivo `cpu_blaze.c` estiver no diretório raiz do projeto

**Nota**: Se a compilação automática falhar, você pode:
- Usar o botão "Compilar C" na interface
- Executar manualmente: `gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall`

## 💻 Como Executar

### Executando a Aplicação

1. **Pelo Visual Studio**:
   - Abra o projeto e pressione `F5`

2. **Pelo executável**:
   - Navegue até `CpuBlazeInterface/CpuBlazeInterface/bin/Debug/net8.0-windows/`
   - Execute `CpuBlazeInterface.exe`
   - Certifique-se de que `cpu_blaze_windows.exe` está no mesmo diretório

3. **Pela linha de comando**:
   ```bash
   cd CpuBlazeInterface/CpuBlazeInterface
   dotnet run
   ```

## 📖 Guia de Uso da Interface

### Tela Principal

A interface é dividida em duas áreas principais:

#### Painel de Controle (Esquerda)

1. **Seleção de Núcleos**
   - Selecione uma opção no dropdown:
     - "Todos os núcleos"
     - "Núcleos ímpares"
     - "Núcleos pares"
     - "Personalizado" (digite os núcleos separados por vírgula, ex: `0,2,4`)

2. **Modo de Operação**
   - **Single**: Cada núcleo usa a porcentagem especificada
   - **Multi**: A porcentagem é dividida entre os núcleos

3. **Porcentagem**
   - Use o controle numérico para definir o nível de carga (1-100%)

4. **Tempo de Execução**
   - Defina a duração do teste em minutos (ex: 1.0 = 1 minuto, 0.5 = 30 segundos)

5. **Rotação de Núcleos** (Opcional)
   - Marque "Ativar Rotação"
   - Defina o tempo em segundos que cada núcleo será utilizado antes de alternar

6. **Botões de Controle**
   - **Iniciar**: Inicia o teste de estresse
   - **Parar**: Interrompe o teste em execução
   - **Compilar C**: Compila o módulo C manualmente (se necessário)

#### Painel de Status (Direita)

- Exibe mensagens do sistema
- Mostra logs de operações em tempo real
- Informa sobre o status do processo de estresse

### Exemplo de Uso

#### Teste Básico
1. Selecione "Todos os núcleos"
2. Escolha modo "Single"
3. Defina porcentagem: 50%
4. Defina tempo: 1.0 minuto
5. Clique em "Iniciar"

#### Teste com Rotação
1. Selecione "Núcleos pares"
2. Escolha modo "Single"
3. Defina porcentagem: 75%
4. Defina tempo: 2.0 minutos
5. Marque "Ativar Rotação" e defina 10 segundos
6. Clique em "Iniciar"

#### Teste Distribuído
1. Selecione "Personalizado" e digite `0,1,2`
2. Escolha modo "Multi"
3. Defina porcentagem: 100%
4. Defina tempo: 1.5 minutos
5. Clique em "Iniciar"
   - Neste caso, 100% será dividido entre 3 núcleos (≈33.3% cada)

## 🏗️ Estrutura do Projeto

```
cpu-blaze/
│
├── CpuBlazeInterface/              # Projeto Windows Forms
│   └── CpuBlazeInterface/
│       ├── Form1.cs                # Interface principal
│       ├── Form1.Designer.cs       # Design da interface
│       ├── CpuBlazeRunner.cs       # Gerenciador de processos
│       ├── CpuBlazeCompiler.cs    # Compilador do módulo C
│       ├── Program.cs              # Ponto de entrada
│       └── CpuBlazeInterface.csproj # Arquivo de projeto
│
├── cpu_blaze.c                     # Módulo C (processamento)
├── cpu_blaze_windows.exe           # Executável compilado (gerado)
│
├── build_cpu_blaze.bat             # Script de compilação (Windows)
├── build_cpu_blaze.ps1             # Script de compilação (PowerShell)
├── build_cpu_blaze.sh              # Script de compilação (Linux/Mac)
│
└── README.md                        # Este arquivo
```

## 🔧 Tecnologias Utilizadas

- **C# .NET 8.0**: Linguagem e framework principal
- **Windows Forms**: Framework de interface gráfica
- **C (GCC)**: Módulo de processamento de baixo nível
- **MSBuild**: Sistema de build do .NET

## 📚 Conceitos de Sistemas Operacionais Demonstrados

Este projeto demonstra os seguintes conceitos:

1. **Gerenciamento de Processos**: Criação e controle de processos externos
2. **Threads e Concorrência**: Uso de múltiplas threads para processamento paralelo
3. **Afinidade de Processador (CPU Affinity)**: Vinculação de threads a núcleos específicos
4. **Sincronização**: Coordenação entre threads e processos
5. **Gerenciamento de Recursos**: Controle de uso de CPU através de porcentagens
6. **Comunicação Interprocesso**: Comunicação entre aplicação C# e módulo C

## ⚠️ Considerações Importantes

- **Uso Responsável**: Esta ferramenta pode gerar alta carga no processador. Use com cuidado e monitore a temperatura do sistema.
- **Permissões**: Algumas funcionalidades podem requerer privilégios administrativos dependendo da configuração do sistema.
- **Desempenho**: Durante os testes, o sistema pode ficar mais lento. Feche aplicações desnecessárias antes de executar testes intensivos.

## 🐛 Solução de Problemas

### Executável não encontrado
- Certifique-se de que `cpu_blaze_windows.exe` está no mesmo diretório da aplicação
- Use o botão "Compilar C" para gerar o executável

### GCC não encontrado
- Instale o MinGW-w64 ou MSYS2
- Adicione o GCC ao PATH do sistema
- Reinicie o Visual Studio após adicionar ao PATH

### Erro ao iniciar processo
- Verifique se não há outro processo de estresse em execução
- Certifique-se de que o executável não está corrompido
- Tente recompilar o módulo C

## 📝 Notas de Desenvolvimento

- O módulo C é compilado automaticamente durante o build se o GCC estiver disponível
- A interface detecta automaticamente o número de núcleos do processador
- Os logs são exibidos em tempo real na interface
- O processo pode ser interrompido a qualquer momento usando o botão "Parar"

## 👥 Autores

Desenvolvido para a disciplina de Sistemas Operacionais.

## 📄 Licença

Este projeto foi desenvolvido para fins acadêmicos.

---

**Versão**: 1.0  
**Última atualização**: 2025
