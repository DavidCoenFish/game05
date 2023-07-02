namespace Automatron
{
   class Program
   {
      private static bool DealTask(ITask in_task, System.Collections.Generic.Dictionary<string, ITask> in_task_map)
      {
         if (true == in_task.HasRun)
         {
            return true;
         }
         else if (true == in_task.Skip)
         {
            System.Console.WriteLine(string.Format("Skip task:{0}", in_task.Name));
            in_task.HasRun = true;
            return true;
         }

         var dependency_task = new System.Collections.Generic.List<ITask>();
         foreach (var dependancy_name in in_task.Dependency)
         {
            if (false == in_task_map.ContainsKey(dependancy_name))
            {
               System.Console.Error.WriteLine(string.Format("Dependancy task:{0} not found for:{1}", dependancy_name, in_task.Name));
               return false;
            }
            var dependancy = in_task_map[dependancy_name];
            dependency_task.Add(dependancy);
            if (false == DealTask(dependancy, in_task_map))
            {
               return false;
            }
         }

         System.Console.WriteLine();
         System.Console.WriteLine(string.Format("Run task:{0}", in_task.Name));
         var result = in_task.Run(dependency_task);
         in_task.HasRun = true;
         return result;
      }

      private static int Run(string[] args)
      {
         if (args.Length < 2)
         {
            System.Console.WriteLine(string.Format("Usage:{0}   Automatron \"root path\" \"abs path to task json\"  [\"commit message\"]", System.Environment.NewLine));
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
         System.Console.WriteLine("Automatron Start:" + System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss"));
         foreach (var arg in args)
         {
            System.Console.WriteLine(arg);
         }

         int result = Run(args);

         System.Console.WriteLine("Automatron End:" + System.DateTime.Now.ToString("yyyy-MM-ddTHH:mm:ss"));
         return result;
      }
   }
}
