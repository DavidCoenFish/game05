using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron
{
   class TaskRunTest : ITask
   {
      public class TaskRunTestPOCO
      {
         public int Priority { get; set; }
         public string[] Dependency { get; set; }
         public bool Skip { get; set; }
      }

      public static ITask Factory(string name, string jsonString, string[] args)
      {
         var data = System.Text.Json.JsonSerializer.Deserialize<TaskRunTestPOCO>(jsonString);
         return new TaskRunTest(
            name,
            data.Priority,
            data.Dependency,
            data.Skip
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
         if ((1 != taskDependants.Count) || (false == (taskDependants[0] is ITaskFile)))
         {
            System.Console.Error.WriteLine("Require a task file type dependency");
            return false;
         }
         ITaskFile dependencyFile = taskDependants[0] as ITaskFile;
         //dependencyFile.FilePath,
         var cmdLine = String.Format(@"""{0}""", dependencyFile.FilePath);
         //var cmdLine = dependencyFile.FilePath;

         //"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\MSTest.exe" /testcontainer:G:\dcoen\UnitTestBuild\UnitTest\x64\Debug\output\UnitTest.dll
         //"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe" G:\dcoen\UnitTestBuild\UnitTest\x64\Debug\output\UnitTest.dll

         bool pass = true;
         if (pass)
         {
            pass = ProcessStart(
               @"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\IDE\CommonExtensions\Microsoft\TestWindow\vstest.console.exe",
               cmdLine
               );
         }

         return pass;
      }

      public TaskRunTest(
         string name,
         int priority,
         string[] dependency,
         bool skip
         )
      {
         _priority = priority;
         _name = name;
         _dependency = dependency;
         _skip = skip;
      }
   }
}
