namespace Automatron
{
   interface ITask
   {
      int Priority
      {
         get;
      }
      string Name
      {
         get;
      }
      string[] Dependency
      {
         get;
      }
      bool Skip
      {
         get;
      }
      bool HasRun
      {
         get;
         set;
      }

      //void Run(System.Collections.Generic.Dictionary<string, ITask> taskCollection);
      bool Run(System.Collections.Generic.List<ITask> in_task_dependency);
      //void Run();
   }
}
