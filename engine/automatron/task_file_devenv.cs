
namespace Automatron
{
   class TaskFileDevenv : ITaskFile
   {
      public class TaskFileDevenvPOCO
      {
         public int Priority { get; set; }
         public string[] Dependency { get; set; }
         public bool Skip { get; set; }
         public bool Clean { get; set; }
         public string DevEnv { get; set; }
         public string FilePath { get; set; }
         public string Solution { get; set; }
         public string Project { get; set; }
         public string Configuration { get; set; }
         public string Platform { get; set; }
      }

      public static TaskFileDevenv Factory(string name, string jsonString, string[] args)
      {
         var rootPath = args[0];
         var data = System.Text.Json.JsonSerializer.Deserialize<TaskFileDevenvPOCO>(jsonString);
         return new TaskFileDevenv(
            name,
            data.Priority,
            data.Dependency,
            data.Skip,
            data.Clean,
            data.DevEnv,
            System.IO.Path.Combine(rootPath, data.FilePath),
            System.IO.Path.Combine(rootPath, data.Solution),
            data.Project,
            data.Configuration,
            data.Platform
            );
      }

      private int _priority;
      public int Priority
      {
         get => _priority;
      }
      private string _name;
      public string Name
      {
         get => _name;
      }
      private string[] _dependency;
      public string[] Dependency
      {
         get => _dependency;
      }
      private bool _skip;
      public bool Skip
      {
         get => _skip;
      }
      private bool _hadRun = false;
      public bool HasRun
      {
         get => _hadRun;
         set => _hadRun = value;
      }
      private string _filePath;
      public string FilePath
      {
         get => _filePath;
      }


      private static void OutputHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.WriteLine(data.Data);
      }
      private static void ErrorHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.Error.WriteLine(data.Data);
      }

      private static bool ProcessStart(string exePath, string armuments)
      {
         System.Console.WriteLine(System.String.Format("ProcessStart: {0} {1}", exePath, armuments));

         var process = new System.Diagnostics.Process();
         process.StartInfo = new System.Diagnostics.ProcessStartInfo(exePath);
         process.StartInfo.UseShellExecute = false;
         process.StartInfo.ErrorDialog = false;
         process.StartInfo.RedirectStandardOutput = true;
         process.StartInfo.RedirectStandardError = true;
         process.StartInfo.Arguments = armuments;
         process.StartInfo.WorkingDirectory = System.IO.Path.GetDirectoryName(exePath);
         process.OutputDataReceived += OutputHandler;
         process.ErrorDataReceived += ErrorHandler;

         if (false == process.Start())
         {
            System.Console.Error.WriteLine(System.String.Format("Could not start process:{0}", exePath));
            System.Environment.Exit(-1);
            return false;
         }
         process.BeginOutputReadLine();
         process.BeginErrorReadLine();
         process.WaitForExit(100000);
         process.Close();
         return true;
      }

      public bool Run(System.Collections.Generic.List<ITask> taskDependants)
      {
         //HasRun = true; ?
         bool pass = true;

         var configuration = _configuration + @"|" + _platform;
         if (pass && _clean)
         {
            pass = ProcessStart(
               _devEnv,
               @"""" + _solution + @""" /clean """ + configuration + @""""
               );
         }

         if (pass)
         {
            pass = ProcessStart(
               _devEnv,
               @"""" + _solution + @""" /build """ + configuration + @"""  /project "+ _project
               );
         }

         return pass;
      }

      public TaskFileDevenv(
         string name,
         int priority,
         string[] dependency,
         bool skip,
         bool clean,
         string devEnv,
         string filePath,
         string solution,
         string project,
         string configuration,
         string platform //Gaming.Desktop.x64
         )
      {
         _priority = priority;
         _name = name;
         _dependency = dependency;
         _skip = skip;
         _clean = clean;
         _devEnv = devEnv;
         _filePath = filePath;
         _solution = solution;
         _project = project;
         _configuration = configuration;
         _platform = platform;
      }

      private bool _clean;
      private string _devEnv;
      private string _solution;
      private string _project;
      private string _configuration;
      private string _platform;

   }
}
