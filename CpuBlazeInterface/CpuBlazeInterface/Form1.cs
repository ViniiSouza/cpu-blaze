namespace CpuBlazeInterface
{
    public partial class Form1 : Form
    {
        private CpuBlazeRunner? _runner;
        private int _totalCores;
        private const string EXECUTABLE_NAME = "cpu_blaze_windows.exe";

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Detectar número de núcleos
            _totalCores = Environment.ProcessorCount;
            
            // Popular ComboBox com opções
            cmbCoreSelection.Items.Add("Todos os núcleos");
            cmbCoreSelection.Items.Add("Núcleos ímpares");
            cmbCoreSelection.Items.Add("Núcleos pares");
            cmbCoreSelection.Items.Add("Personalizado");
            cmbCoreSelection.SelectedIndex = 0;
            
            UpdateSelectedCoresDisplay();
            AppendStatus($"Sistema detectado: {_totalCores} núcleos de processador disponíveis.");
            
            // Encontrar o executável
            string? executablePath = FindExecutable();
            if (executablePath != null)
            {
                _runner = new CpuBlazeRunner(executablePath);
                _runner.OutputReceived += Runner_OutputReceived;
                _runner.ProcessExited += Runner_ProcessExited;
                AppendStatus($"Executável encontrado: {executablePath}");
            }
            else
            {
                AppendStatus("ERRO: Executável cpu_blaze_windows.exe não encontrado!");
                AppendStatus("Certifique-se de que o arquivo está no mesmo diretório da aplicação.");
                btnStart.Enabled = false;
            }
        }

        private string? FindExecutable()
        {
            // Procurar no diretório da aplicação
            string appDir = Application.StartupPath;
            string exePath = Path.Combine(appDir, EXECUTABLE_NAME);
            if (File.Exists(exePath))
                return exePath;

            // Procurar no diretório pai (bin/Debug/net8.0-windows -> bin -> Debug -> net8.0-windows -> CpuBlazeInterface -> CpuBlazeInterface)
            // Subir até o diretório raiz do projeto
            DirectoryInfo? currentDir = new DirectoryInfo(appDir);
            for (int i = 0; i < 5 && currentDir != null; i++)
            {
                exePath = Path.Combine(currentDir.FullName, EXECUTABLE_NAME);
                if (File.Exists(exePath))
                    return exePath;
                currentDir = currentDir.Parent;
            }

            return null;
        }

        private void CmbCoreSelection_SelectedIndexChanged(object sender, EventArgs e)
        {
            UpdateSelectedCoresDisplay();
        }

        private void UpdateSelectedCoresDisplay()
        {
            List<int> cores = GetSelectedCores();
            txtSelectedCores.Text = cores.Count > 0 ? string.Join(",", cores) : "";
            txtSelectedCores.Enabled = cmbCoreSelection.SelectedIndex == 3; // Personalizado
        }

        private List<int> GetSelectedCores()
        {
            List<int> cores = new List<int>();

            switch (cmbCoreSelection.SelectedIndex)
            {
                case 0: // Todos
                    for (int i = 0; i < _totalCores; i++)
                        cores.Add(i);
                    break;
                case 1: // Ímpares
                    for (int i = 0; i < _totalCores; i++)
                        if (i % 2 == 1)
                            cores.Add(i);
                    break;
                case 2: // Pares
                    for (int i = 0; i < _totalCores; i++)
                        if (i % 2 == 0)
                            cores.Add(i);
                    break;
                case 3: // Personalizado
                    string text = txtSelectedCores.Text.Trim();
                    if (!string.IsNullOrEmpty(text))
                    {
                        string[] parts = text.Split(',');
                        foreach (string part in parts)
                        {
                            if (int.TryParse(part.Trim(), out int core) && core >= 0 && core < _totalCores)
                                cores.Add(core);
                        }
                    }
                    break;
            }

            return cores;
        }

        private void ChkRotation_CheckedChanged(object sender, EventArgs e)
        {
            numRotationSeconds.Enabled = chkRotation.Checked;
        }

        private void BtnStart_Click(object sender, EventArgs e)
        {
            if (_runner == null)
            {
                MessageBox.Show("Executável não encontrado!", "Erro", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (_runner.IsRunning)
            {
                MessageBox.Show("Processo já está em execução!", "Aviso", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            List<int> cores = GetSelectedCores();
            if (cores.Count == 0)
            {
                MessageBox.Show("Selecione pelo menos um núcleo!", "Erro", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            CpuBlazeParameters parameters = new CpuBlazeParameters
            {
                Cores = cores,
                TimeMinutes = (double)numTimeMinutes.Value,
                Percent = (int)numPercent.Value,
                Mode = rbModeSingle.Checked ? "single" : "multi",
                RotationSeconds = chkRotation.Checked ? (int)numRotationSeconds.Value : 0
            };

            try
            {
                AppendStatus($"\n=== Iniciando CPU Blaze ===");
                AppendStatus($"Núcleos: {string.Join(", ", cores)}");
                AppendStatus($"Modo: {parameters.Mode}");
                AppendStatus($"Porcentagem: {parameters.Percent}%");
                AppendStatus($"Tempo: {parameters.TimeMinutes:F2} minutos");
                if (parameters.RotationSeconds > 0)
                    AppendStatus($"Rotação: {parameters.RotationSeconds} segundos por núcleo");
                
                _runner.Start(parameters);
                
                btnStart.Enabled = false;
                btnStop.Enabled = true;
            }
            catch (Exception ex)
            {
                AppendStatus($"ERRO ao iniciar: {ex.Message}");
                MessageBox.Show($"Erro ao iniciar processo: {ex.Message}", "Erro", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void BtnStop_Click(object sender, EventArgs e)
        {
            if (_runner != null && _runner.IsRunning)
            {
                AppendStatus("\n=== Parando processo ===");
                _runner.Stop();
                AppendStatus("Processo parado.");
            }
        }

        private void Runner_OutputReceived(object? sender, string output)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() => AppendStatus(output)));
            }
            else
            {
                AppendStatus(output);
            }
        }

        private void Runner_ProcessExited(object? sender, EventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new Action(() =>
                {
                    btnStart.Enabled = true;
                    btnStop.Enabled = false;
                    AppendStatus("\n=== Processo finalizado ===");
                }));
            }
            else
            {
                btnStart.Enabled = true;
                btnStop.Enabled = false;
                AppendStatus("\n=== Processo finalizado ===");
            }
        }

        private void AppendStatus(string text)
        {
            txtStatus.AppendText($"[{DateTime.Now:HH:mm:ss}] {text}\n");
            txtStatus.ScrollToCaret();
        }
    }
}
