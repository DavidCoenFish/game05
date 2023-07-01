namespace Automatron03
{
   class Program
   {
      private static bool DealTask(ITask task, System.Collections.Generic.Dictionary<string, ITask> taskMap)
      {
         if (true == task.HasRun)
         {
            return true;
         }
         else if (true == task.Skip)
         {
            System.Console.WriteLine(string.Format("Skip task:{0}", task.Name));
            task.HasRun = true;
            return true;
         }

         var dependencyTask = new System.Collections.Generic.List<ITask>();
         foreach (var dependancyName in task.Dependency)
         {
            if (false == taskMap.ContainsKey(dependancyName))
            {
               System.Console.Error.WriteLine(string.Format("Dependancy task:{0} not found for:{1}", dependancyName, task.Name));
               return false;
            }
            var dependancy = taskMap[dependancyName];
            dependencyTask.Add(dependancy);
            if (false == DealTask(dependancy, taskMap))
            {
               return false;
            }
         }

         System.Console.WriteLine();
         System.Console.WriteLine(string.Format("Run task:{0}", task.Name));
         var result = task.Run(dependencyTask);
         task.HasRun = true;
         return result;
      }

      private static int Run(string[] args)
      {
         if (args.Length < 2)
         {
            System.Console.WriteLine(string.Format("Usage:{0}   Automatron03 \"root path\" \"abs path to task json\"  [\"commit message\"]", System.Environment.NewLine));
            return -1;
         }
         //var rootPath = args[0];
         var pathToJson = args[1];
         if (false == System.IO.Directory.Exists(pathToJson))
         {
            System.Console.Error.WriteLine(string.Format("Directory:{0} does not exists", pathToJson));
            return -1;
         }

         //load the tasks
         var taskList = new System.Collections.Generic.List<ITask>();
         var taskMap = new System.Collections.Generic.Dictionary<string, ITask>();
         string[] fileEntries = System.IO.Directory.GetFiles(pathToJson);
         foreach (string fileName in fileEntries)
         {
            //System.Console.WriteLine(fileName);
            var task = Task.DealFile(fileName, args);
            if (null != task)
            {
               taskList.Add(task);
               taskMap.Add(task.Name, task);
            }
         }

         //sort tasks
         taskList.Sort((x, y) => x.Priority.CompareTo(y.Priority));

         //deal with the tasks
         foreach (var task in taskList)
         {
            if (false == DealTask(task, taskMap))
            {
               return -1;
            }
         }
         return 0;
      }

      //args = ["G:\dcoen\game02", "G:\dcoen\game02\Automatron03\Tasks", "test commit"]
      //C:\development\game04
      //C:\development\game04\support\s01_d3dx12_engine\Automatron03\TasksTest
      static int Main(string[] args)
      {
         System.Console.WriteLine("Automatron03 Start:" + System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss"));
         foreach (var arg in args)
         {
            System.Console.WriteLine(arg);
         }

         int result = Run(args);

         System.Console.WriteLine("Automatron03 End:" + System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss"));
         return result;
      }
   }
}
