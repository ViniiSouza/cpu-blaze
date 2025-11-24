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

### Hardware
- Processador com múltiplos núcleos (recomendado)
- Mínimo 4 GB de RAM
- Espaço em disco: ~50 MB

## 🚀 Instalação e Compilação

### Pré-requisitos

1. **Instalar .NET 8.0 SDK**
   - Baixe em: https://dotnet.microsoft.com/download/dotnet/8.0
   - Execute o instalador e siga as instruções
   - Verifique a instalação executando `dotnet --version` no terminal

2. **Instalar GCC (GNU Compiler Collection)**
   
   O GCC é necessário para compilar o módulo C que faz o processamento de estresse da CPU.
   
   **Opção A: MinGW-w64 (Recomendado)**
   - Baixe o instalador em: https://www.mingw-w64.org/downloads/
   - Ou use o instalador automático: https://sourceforge.net/projects/mingw-w64/
   - Durante a instalação, certifique-se de marcar a opção "Add to PATH"
   - Reinicie o terminal/Visual Studio após a instalação
   
   **Opção B: MSYS2**
   - Baixe em: https://www.msys2.org/
   - Após instalar, abra o terminal MSYS2 e execute:
     ```bash
     pacman -S mingw-w64-x86_64-gcc
     ```
   - Adicione `C:\msys64\mingw64\bin` ao PATH do sistema
   
   **Verificar instalação:**
   - Abra um novo terminal (PowerShell ou CMD)
   - Execute: `gcc --version`
   - Deve exibir a versão do GCC instalada

3. **Instalar Visual Studio 2022** (opcional, para desenvolvimento)
   - Inclua o workload "Desenvolvimento para desktop com .NET"
   - Inclua o componente "Windows Forms"

### Compilação do Projeto

#### Passo 1: Compilar o Módulo C

Antes de compilar a interface, você precisa compilar o módulo C que faz o processamento:

**Opção A: Compilação Automática (Recomendado)**

O projeto C# compila automaticamente o código C durante o build se o GCC estiver no PATH:

```bash
# Navegue até o diretório raiz do projeto
cd cpu-blaze

# Compile o código C manualmente (se a compilação automática falhar)
gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall
```

**Opção B: Usar Script de Compilação**

Execute o script fornecido:

```bash
# No diretório raiz do projeto
.\build_cpu_blaze.bat
```

**Verificar:**
- Certifique-se de que o arquivo `cpu_blaze_windows.exe` foi criado no diretório raiz do projeto

#### Passo 2: Compilar a Interface C#

#### Método 1: Visual Studio (Recomendado)

1. Abra o arquivo `CpuBlazeInterface.slnx` no Visual Studio 2022
2. Aguarde a restauração dos pacotes NuGet
3. O projeto tentará compilar automaticamente o módulo C (se o GCC estiver disponível)
4. Pressione `F5` para executar ou `Ctrl+Shift+B` para compilar
5. Se a compilação automática do C falhar, compile manualmente usando o Passo 1

#### Método 2: Linha de Comando

```bash
# Navegue até o diretório do projeto
cd CpuBlazeInterface/CpuBlazeInterface

# Restaure as dependências
dotnet restore

# Compile o projeto (tentará compilar o C automaticamente se GCC estiver disponível)
dotnet build

# Se o build mostrar aviso sobre GCC, compile o C manualmente:
# cd ..\..
# gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall
# cd CpuBlazeInterface/CpuBlazeInterface

# Execute a aplicação
dotnet run
```

#### Método 3: Compilação do Executável

```bash
# Compile para produção
dotnet publish -c Release -r win-x64 --self-contained false

# O executável estará em:
# CpuBlazeInterface/CpuBlazeInterface/bin/Release/net8.0-windows/win-x64/publish/

# IMPORTANTE: Copie o cpu_blaze_windows.exe para o mesmo diretório do executável
# ou a aplicação não funcionará corretamente
```

## 💻 Como Executar

### Executando a Aplicação

**IMPORTANTE:** Certifique-se de que o arquivo `cpu_blaze_windows.exe` está no mesmo diretório do executável da interface ou no diretório raiz do projeto.

1. **Pelo Visual Studio**:
   - Abra o projeto e pressione `F5`
   - A aplicação procurará automaticamente o `cpu_blaze_windows.exe`

2. **Pelo executável**:
   - Navegue até `CpuBlazeInterface/CpuBlazeInterface/bin/Debug/net8.0-windows/`
   - Certifique-se de que `cpu_blaze_windows.exe` está no mesmo diretório
   - Execute `CpuBlazeInterface.exe`

3. **Pela linha de comando**:
   ```bash
   cd CpuBlazeInterface/CpuBlazeInterface
   dotnet run
   ```

### Resumo do Processo Completo

1. Instalar .NET 8.0 SDK
2. Instalar GCC (MinGW-w64 ou MSYS2)
3. Verificar GCC no PATH (`gcc --version`)
4. Compilar o módulo C (`gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall`)
5. Compilar a interface C# (`dotnet build`)
6. Executar a aplicação

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
│       ├── CpuBlazeCompiler.cs    # Utilitário de compilação
│       ├── Program.cs              # Ponto de entrada
│       └── CpuBlazeInterface.csproj # Arquivo de projeto
│
└── README.md                        # Este arquivo
```

## 🔧 Tecnologias Utilizadas

- **C# .NET 8.0**: Linguagem e framework principal
- **Windows Forms**: Framework de interface gráfica
- **MSBuild**: Sistema de build do .NET

## 📚 Conceitos de Sistemas Operacionais Demonstrados

Este projeto demonstra os seguintes conceitos:

1. **Gerenciamento de Processos**: Criação e controle de processos externos
2. **Threads e Concorrência**: Uso de múltiplas threads para processamento paralelo
3. **Afinidade de Processador (CPU Affinity)**: Vinculação de threads a núcleos específicos
4. **Sincronização**: Coordenação entre threads e processos
5. **Gerenciamento de Recursos**: Controle de uso de CPU através de porcentagens
6. **Comunicação Interprocesso**: Comunicação entre componentes da aplicação

## ⚠️ Considerações Importantes

- **Uso Responsável**: Esta ferramenta pode gerar alta carga no processador. Use com cuidado e monitore a temperatura do sistema.
- **Permissões**: Algumas funcionalidades podem requerer privilégios administrativos dependendo da configuração do sistema.
- **Desempenho**: Durante os testes, o sistema pode ficar mais lento. Feche aplicações desnecessárias antes de executar testes intensivos.

## 🐛 Solução de Problemas

### GCC não encontrado
- **Erro:** "GCC não encontrado" ou "gcc: comando não reconhecido"
- **Solução:**
  1. Verifique se o GCC está instalado: `gcc --version`
  2. Se não estiver instalado, instale MinGW-w64 ou MSYS2
  3. Adicione o GCC ao PATH do sistema:
     - MinGW: Adicione `C:\mingw64\bin` (ou caminho da instalação) ao PATH
     - MSYS2: Adicione `C:\msys64\mingw64\bin` ao PATH
  4. Reinicie o terminal e o Visual Studio após adicionar ao PATH
  5. Compile o código C manualmente: `gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall`

### Executável cpu_blaze_windows.exe não encontrado
- **Erro:** "Executável cpu_blaze_windows.exe não encontrado!"
- **Solução:**
  1. Compile o código C manualmente:
     ```bash
     cd cpu-blaze
     gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall
     ```
  2. Certifique-se de que o arquivo está no diretório raiz do projeto
  3. Ou copie o executável para o mesmo diretório do `CpuBlazeInterface.exe`

### Erro ao iniciar processo
- Verifique se não há outro processo de estresse em execução
- Certifique-se de que o `cpu_blaze_windows.exe` está presente e não está corrompido
- Tente recompilar o módulo C

### Aplicação não inicia
- Verifique se o .NET 8.0 Runtime está instalado
- Certifique-se de que está usando Windows 10/11
- Verifique os logs de erro no Visual Studio
- Certifique-se de que o `cpu_blaze_windows.exe` foi compilado corretamente

### Erro de compilação do código C
- Verifique se o GCC está funcionando: `gcc --version`
- Verifique se há erros de sintaxe no `cpu_blaze.c`
- Tente compilar manualmente para ver mensagens de erro detalhadas:
  ```bash
  gcc cpu_blaze.c -o cpu_blaze_windows.exe -Wall -v
  ```

## 📝 Notas de Desenvolvimento

- A interface detecta automaticamente o número de núcleos do processador
- Os logs são exibidos em tempo real na interface
- O processo pode ser interrompido a qualquer momento usando o botão "Parar"
- A aplicação gerencia automaticamente os recursos necessários

## 👥 Autores

Desenvolvido para a disciplina de Sistemas Operacionais.

## 📄 Licença

Este projeto foi desenvolvido para fins acadêmicos.

---

**Versão**: 1.0  
**Última atualização**: 2025
