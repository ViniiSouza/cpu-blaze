using System.Diagnostics;
using System.Text;

namespace CpuBlazeInterface
{
    public class CpuBlazeRunner
    {
        private Process? _process;
        private readonly string _executablePath;

        public CpuBlazeRunner(string executablePath)
        {
            _executablePath = executablePath;
        }

        public bool IsRunning => _process != null && !_process.HasExited;

        public event EventHandler<string>? OutputReceived;
        public event EventHandler? ProcessExited;

        public void Start(CpuBlazeParameters parameters)
        {
            if (IsRunning)
            {
                throw new InvalidOperationException("Processo já está em execução.");
            }

            string arguments = BuildArguments(parameters);

            _process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = _executablePath,
                    Arguments = arguments,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8,
                    StandardErrorEncoding = Encoding.UTF8
                }
            };

            _process.OutputDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    OutputReceived?.Invoke(this, e.Data);
                }
            };

            _process.ErrorDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    OutputReceived?.Invoke(this, e.Data);
                }
            };

            _process.Exited += (sender, e) =>
            {
                ProcessExited?.Invoke(this, EventArgs.Empty);
            };

            _process.EnableRaisingEvents = true;

            _process.Start();
            _process.BeginOutputReadLine();
            _process.BeginErrorReadLine();
        }

        public void Stop()
        {
            if (_process == null)
                return;

            try
            {
                if (_process.HasExited)
                {
                    CleanupProcess();
                    return;
                }

                try
                {
                    if (!_process.HasExited)
                    {
                        _process.CloseMainWindow();
                        if (!_process.WaitForExit(500))
                        {
                            _process.Kill();
                        }
                    }
                }
                catch (InvalidOperationException)
                {
                    if (!_process.HasExited)
                    {
                        _process.Kill();
                    }
                }

                if (!_process.HasExited)
                {
                    _process.WaitForExit(3000);
                }

                if (!_process.HasExited)
                {
                    try
                    {
                        _process.Kill();
                        _process.WaitForExit(2000);
                    }
                    catch { }
                }
            }
            catch (Exception ex)
            {
                OutputReceived?.Invoke(this, $"Erro ao parar processo: {ex.Message}");
            }
            finally
            {
                CleanupProcess();
            }
        }

        private void CleanupProcess()
        {
            try
            {
                if (_process != null)
                {
                    if (!_process.HasExited)
                    {
                        try { _process.Kill(); } catch { }
                    }

                    try
                    {
                        _process.CancelOutputRead();
                        _process.CancelErrorRead();
                    }
                    catch { }

                    try
                    {
                        _process.Close();
                    }
                    catch { }

                    try
                    {
                        _process.Dispose();
                    }
                    catch { }
                }
            }
            catch { }
            finally
            {
                _process = null;
                ProcessExited?.Invoke(this, EventArgs.Empty);
            }
        }

        private string BuildArguments(CpuBlazeParameters parameters)
        {
            var args = new StringBuilder();

            // Núcleos
            if (parameters.Cores != null && parameters.Cores.Count > 0)
            {
                args.Append($"-c {string.Join(",", parameters.Cores)} ");
            }

            // Tempo
            if (parameters.TimeMinutes > 0)
            {
                args.Append($"-t {parameters.TimeMinutes:F2} ");
            }

            // Porcentagem
            if (parameters.Percent >= 0 && parameters.Percent <= 100)
            {
                args.Append($"-p {parameters.Percent} ");
            }

            // Modo
            args.Append($"-m {parameters.Mode} ");

            // Rotação (se especificado)
            if (parameters.RotationSeconds > 0)
            {
                args.Append($"-r {parameters.RotationSeconds} ");
            }

            return args.ToString().Trim();
        }
    }

    public class CpuBlazeParameters
    {
        public List<int>? Cores { get; set; }
        public double TimeMinutes { get; set; } = 1.0;
        public int Percent { get; set; } = 100;
        public string Mode { get; set; } = "single";
        public int RotationSeconds { get; set; } = 0; // 0 = sem rotação
    }
}
