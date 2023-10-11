using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron
{
   /// Extent the task interface to add a file path for tasks representing a file
   interface ITaskFile : ITask
   {
      /// path to a file system file
      string FilePath
      {
         get;
      }
   }
}
