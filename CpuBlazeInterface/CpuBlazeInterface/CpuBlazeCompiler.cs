using System.Diagnostics;
using System.Text;

namespace CpuBlazeInterface
{
    public static class CpuBlazeCompiler
    {
        private const string SOURCE_FILE = "cpu_blaze.c";
        private const string OUTPUT_FILE = "cpu_blaze_windows.exe";

        /// <summary>
        /// Verifica se o GCC está disponível no sistema
        /// </summary>
        public static bool IsGccAvailable()
        {
            try
            {
                ProcessStartInfo psi = new ProcessStartInfo
                {
                    FileName = "gcc",
                    Arguments = "--version",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                };

                using (Process? process = Process.Start(psi))
                {
                    if (process != null)
                    {
                        process.WaitForExit(3000);
                        return process.ExitCode == 0;
                    }
                }
            }
            catch
            {
                // GCC não encontrado
            }

            return false;
        }

        /// <summary>
        /// Compila o código C e gera o executável
        /// </summary>
        /// <param name="sourceDirectory">Diretório onde está o arquivo cpu_blaze.c</param>
        /// <param name="outputDirectory">Diretório onde será gerado o executável (null = mesmo diretório do fonte)</param>
        /// <param name="progressCallback">Callback para receber mensagens de progresso</param>
        /// <returns>True se a compilação foi bem-sucedida</returns>
        public static bool Compile(string sourceDirectory, string? outputDirectory = null, Action<string>? progressCallback = null)
        {
            string sourcePath = Path.Combine(sourceDirectory, SOURCE_FILE);
            outputDirectory ??= sourceDirectory;
            string outputPath = Path.Combine(outputDirectory, OUTPUT_FILE);

            if (!File.Exists(sourcePath))
            {
                progressCallback?.Invoke($"ERRO: Arquivo não encontrado: {sourcePath}");
                return false;
            }

            if (!IsGccAvailable())
            {
                progressCallback?.Invoke("ERRO: GCC não encontrado no PATH. Instale MinGW ou adicione o GCC ao PATH.");
                return false;
            }

            progressCallback?.Invoke($"Compilando {SOURCE_FILE}...");

            try
            {
                ProcessStartInfo psi = new ProcessStartInfo
                {
                    FileName = "gcc",
                    Arguments = $"\"{sourcePath}\" -o \"{outputPath}\" -Wall",
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    WorkingDirectory = sourceDirectory
                };

                using (Process process = Process.Start(psi)!)
                {
                    StringBuilder output = new StringBuilder();
                    StringBuilder error = new StringBuilder();

                    process.OutputDataReceived += (sender, e) =>
                    {
                        if (!string.IsNullOrEmpty(e.Data))
                            output.AppendLine(e.Data);
                    };

                    process.ErrorDataReceived += (sender, e) =>
                    {
                        if (!string.IsNullOrEmpty(e.Data))
                            error.AppendLine(e.Data);
                    };

                    process.BeginOutputReadLine();
                    process.BeginErrorReadLine();

                    process.WaitForExit();

                    if (process.ExitCode == 0)
                    {
                        progressCallback?.Invoke($"Compilação concluída com sucesso!");
                        progressCallback?.Invoke($"Executável gerado: {outputPath}");
                        
                        if (File.Exists(outputPath))
                        {
                            FileInfo fileInfo = new FileInfo(outputPath);
                            progressCallback?.Invoke($"Tamanho: {fileInfo.Length} bytes");
                        }

                        return true;
                    }
                    else
                    {
                        progressCallback?.Invoke($"ERRO na compilação (código: {process.ExitCode})");
                        if (error.Length > 0)
                            progressCallback?.Invoke($"Erros:\n{error}");
                        if (output.Length > 0)
                            progressCallback?.Invoke($"Output:\n{output}");
                        return false;
                    }
                }
            }
            catch (Exception ex)
            {
                progressCallback?.Invoke($"ERRO ao executar GCC: {ex.Message}");
                return false;
            }
        }

        /// <summary>
        /// Verifica se o executável precisa ser recompilado
        /// </summary>
        public static bool NeedsRecompilation(string sourceDirectory, string? outputDirectory = null)
        {
            string sourcePath = Path.Combine(sourceDirectory, SOURCE_FILE);
            outputDirectory ??= sourceDirectory;
            string outputPath = Path.Combine(outputDirectory, OUTPUT_FILE);

            if (!File.Exists(sourcePath))
                return false;

            if (!File.Exists(outputPath))
                return true;

            DateTime sourceTime = File.GetLastWriteTime(sourcePath);
            DateTime outputTime = File.GetLastWriteTime(outputPath);

            return sourceTime > outputTime;
        }
    }
}


