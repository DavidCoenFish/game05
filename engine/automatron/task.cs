namespace Automatron
{
   class Task
   {
      private static bool _initialized = false;
      private static System.Collections.Generic.Dictionary<string, System.Func<string, string, string[], ITask>> _taskFactoryMap;
      private static System.Collections.Generic.Dictionary<string, System.Func<string, string, string[], ITask>> TaskFactoryMap
      {
         get
         {
            if (_initialized)
            {
               return _taskFactoryMap;
            }
            _taskFactoryMap = new System.Collections.Generic.Dictionary<string, System.Func<string, string, string[], ITask>> ();
            _taskFactoryMap.Add("TaskFileDevenv", TaskFileDevenv.Factory);
            _taskFactoryMap.Add("TaskRunExeAction", TaskRunExeAction.Factory);
            _taskFactoryMap.Add("TaskRunTest", TaskRunTest.Factory);
            _taskFactoryMap.Add("TaskGitCommit", TaskGitCommit.Factory);
            _initialized = true;
            return _taskFactoryMap;
         }
      }

      public class TaskPOCO
      {
         public string Factory { get; set; }
      }

      public static ITask DealFile(string fileName, string[] args)
      {
         ITask result = null;

         string name = System.IO.Path.GetFileNameWithoutExtension(fileName);

         System.Console.WriteLine(fileName);

         var jsonString = System.IO.File.ReadAllText(fileName, System.Text.Encoding.UTF8);
         var data = System.Text.Json.JsonSerializer.Deserialize<TaskPOCO>(jsonString);
         if (TaskFactoryMap.ContainsKey(data.Factory))
         {
            var function = TaskFactoryMap[data.Factory];
            result = function(name, jsonString, args);
         }
         else
         {
            System.Console.Error.WriteLine(string.Format("Request for unknown taks:{0}", data.Factory));
         }

         return result;
      }
   }
}
