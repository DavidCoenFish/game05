namespace Automatron
{
   /// Base class for the Automatron exe to perform
   interface ITask
   {
      /// hint for sorting tasks, bigger number is sorted infront of small
      int Priority
      {
         get;
      }
      /// Name of the task, for dependencies to reference
      string Name
      {
         get;
      }
      /// The names of task that need to be run before we are
      string[] Dependency
      {
         get;
      }
      /// Convienience of disabling a task without fully removing from data files
      bool Skip
      {
         get;
      }
      /// keep track of if the given task has been run
      bool HasRun
      {
         get;
         set;
      }

      /// Perform the dependencies, and then the task itself
      bool Run(System.Collections.Generic.List<ITask> in_task_dependency);

   }
}
