using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron03
{
   class TaskGitCommit : ITask
   {
      public class TaskGitCommitPOCO
      {
         public int Priority { get; set; }
         public string[] Dependency { get; set; }
         public bool Skip { get; set; }
         public string Git { get; set; }
      }

      public static ITask Factory(string name, string jsonString, string[] args)
      {
         var rootPath = args[0];
         var commitMessage = "";
         if (2 < args.Length)
         {
            commitMessage = args[2];
         }
         if (commitMessage == "")
         {
            commitMessage = String.Format(@"Automatron auto generated commit message {0}", System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss"));
         }

         var data = System.Text.Json.JsonSerializer.Deserialize<TaskGitCommitPOCO>(jsonString);
         return new TaskGitCommit(
            rootPath,
            name,
            data.Priority,
            data.Dependency,
            data.Skip,
            data.Git,
            commitMessage
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

      private static void OutputHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.WriteLine(data.Data);
      }
      private static void ErrorHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.Error.WriteLine(data.Data);
      }

      private static bool ProcessStart(string workingDir, string exePath, string armuments)
      {
         System.Console.WriteLine(System.String.Format("ProcessStart: {0} {1} {2}", workingDir, exePath, armuments));

         var process = new System.Diagnostics.Process();
         process.StartInfo = new System.Diagnostics.ProcessStartInfo(exePath);
         process.StartInfo.UseShellExecute = false;
         process.StartInfo.ErrorDialog = false;
         process.StartInfo.RedirectStandardOutput = true;
         process.StartInfo.RedirectStandardError = true;
         process.StartInfo.Arguments = armuments;
         process.StartInfo.WorkingDirectory = workingDir;
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
         //git pull <REMOTE> <name-of-branch [master]>
         //git pull origin master
         //git remote -v
         //git status
         //git add .
         //git commit -m "COMMENT TO DESCRIBE THE INTENTION OF THE COMMIT"
         //git push <remote> <name-of-branch>
         //git push origin master

         bool pass = true;
         if (pass)
         {
            pass = ProcessStart(_rootPath, _git, "pull");
         }
         if (pass)
         {
            pass = ProcessStart(_rootPath, _git, "add .");
         }
         if (pass)
         {
            pass = ProcessStart(_rootPath, _git, String.Format(@"commit -m ""{0}""", _commitMessage));
         }
         if (pass)
         {
            pass = ProcessStart(_rootPath, _git, "push");
         }

         return pass;
      }

      public TaskGitCommit(
         string rootPath,
         string name,
         int priority,
         string[] dependency,
         bool skip,
         string git,
         string commitMessage
         )
      {
         _rootPath = rootPath;
         _priority = priority;
         _name = name;
         _dependency = dependency;
         _skip = skip;
         _git = git;
         _commitMessage = commitMessage;
      }

      private string _rootPath;
      private string _git;
      private string _commitMessage;
   }
}
