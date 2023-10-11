using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron
{
   /// Run the file of the first dependency as an exe
   class TaskRunExeAction : ITask
   {
      /// Plain old class object for json reflection
      /// extra data for a task
      public class TaskRunExeActionPOCO
      {
         public int Priority { get; set; }
         public string[] Dependency { get; set; }
         public bool Skip { get; set; }
         public string CommandLine { get; set; }
         public ActionPOCO[] Actions { get; set; }
      }

      /// Plain old class object for json reflection
      /// define an action
      public class ActionPOCO
      {
         public string Factory { get; set; }
         public string StringParam0 { get; set; }
         public int IntParam0 { get; set; }

      }

      private static int GetVkKey(string key)
      {
         int result = 0;
         switch (key)
         {
            default:
               break;
            case "VK_RETURN":
               result = VK_RETURN;
               break;
         }
         return result;
      }

      /// Generate a task object with given name and actions from the given json string
      public static ITask Factory(string name, string jsonString, string[] args)
      {
         var data = System.Text.Json.JsonSerializer.Deserialize<TaskRunExeActionPOCO>(jsonString);
         var actionList = new System.Collections.Generic.List< System.Action<System.Diagnostics.Process>>();
         if (null != data.Actions)
         {
            foreach (var item in data.Actions)
            {
               switch (item.Factory)
               {
                  default:
                     break;
                  case "Wait":
                     actionList.Add(ActionFactoryWait(item.IntParam0));
                     break;
                  case "Close":
                     actionList.Add(ActionFactoryPostMessage(WM_CLOSE, System.IntPtr.Zero, System.IntPtr.Zero));
                     break;
                  case "SysKeyDown":
                     actionList.Add(ActionFactoryPostMessage(WM_SYSKEYDOWN, new System.IntPtr(GetVkKey(item.StringParam0)), System.IntPtr.Zero));
                     break;
                  case "SysKeyDownAlt":
                     actionList.Add(ActionFactoryPostMessage(WM_SYSKEYDOWN, new System.IntPtr(GetVkKey(item.StringParam0)), new System.IntPtr(KEY_STATE_ALT_DOWN)));
                     break;
               }
            }
         }

         return new TaskRunExeAction(
            name,
            data.Priority,
            data.Dependency,
            data.Skip,
            data.CommandLine,
            actionList
            );
      }

      [System.Runtime.InteropServices.DllImport("user32.dll", SetLastError = true)]
      public static extern bool PostMessage(System.IntPtr hWnd, int Msg, System.IntPtr wParam, System.IntPtr lParam);
      const int WM_SYSKEYDOWN = 0x0104;
      const int VK_RETURN = 0x0D;
      const long KEY_STATE_ALT_DOWN = 0x20000000;
      const int WM_CLOSE = 0x0010;

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

      static System.Action<System.Diagnostics.Process> ActionFactoryWait(int wait)
      {
         return (System.Diagnostics.Process process) =>
         {
            System.Console.WriteLine("ActionFactoryWait:{0}", wait);
            System.Threading.Thread.Sleep(wait);
         };
      }

      static System.Action<System.Diagnostics.Process> ActionFactoryPostMessage(int Msg, System.IntPtr wParam, System.IntPtr lParam)
      {
         return (System.Diagnostics.Process process) =>
         {
            System.Console.WriteLine("ActionFactoryPostMessage:{0} {1} {2}", Msg, wParam, lParam);
            PostMessage(process.MainWindowHandle, Msg, wParam, lParam);
         };
      }

      private static void OutputHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.WriteLine(data.Data);
      }
      private static void ErrorHandler(object sendingProcess, System.Diagnostics.DataReceivedEventArgs data)
      {
         System.Console.Error.WriteLine(data.Data);
      }

      private static bool ProcessStart(string exePath, string armuments, System.Collections.Generic.List< System.Action<System.Diagnostics.Process>> actionList)
      {
         System.Console.WriteLine(System.String.Format("ProcessStart: {0} {1}", exePath, armuments));

         /*
         having a problem with operating system 
The process cannot access the file because it is being used by another process
         */
         System.Threading.Thread.Sleep(500);

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

         foreach (var item in actionList)
         {
            item(process);
         }

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

         bool pass = true;
         if (pass)
         {
            pass = ProcessStart(
               dependencyFile.FilePath,
               _comandLine,
               _actionList
               );
         }

         return pass;
      }

      public TaskRunExeAction(
         string name,
         int priority,
         string[] dependency,
         bool skip,
         string comandLine,
         System.Collections.Generic.List<System.Action<System.Diagnostics.Process>> actionList
         )
      {
         _priority = priority;
         _name = name;
         _dependency = dependency;
         _skip = skip;
         _comandLine = comandLine;
         _actionList = actionList;
      }

      private string _comandLine;
      private System.Collections.Generic.List<System.Action<System.Diagnostics.Process>> _actionList;
   }
}
