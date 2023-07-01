using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Automatron03
{
   interface ITaskFile : ITask
   {
      string FilePath
      {
         get;
      }
   }
}
