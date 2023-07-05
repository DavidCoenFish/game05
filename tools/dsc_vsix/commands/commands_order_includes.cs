using System;
using Community.VisualStudio.Toolkit;
using Microsoft.VisualStudio.Shell;
using Task = System.Threading.Tasks.Task;

namespace dsc_vsix
{
    [Command(PackageIds._commands_order_includes)]
    internal sealed class CommandsOrderIncludes : BaseCommand<CommandsOrderIncludes>
    {
        protected override async Task ExecuteAsync(OleMenuCmdEventArgs e)
        {
        /*
            DocumentView doc_view = await VS.Documents.GetActiveDocumentViewAsync();
            var position = doc_view.TextView?.Selection.Start.Position.Position;

            if (position.HasValue)
            {
                doc_view.TextBuffer.Insert(position.Value, Guid.NewGuid().ToString());
            }
        */
        //    var document_window_list = await VS.Windows.GetAllDocumentWindowsAsync();
            DocumentView doc_view = await VS.Documents.GetActiveDocumentViewAsync();
            var lines = doc_view.TextView?.Selection.TextView.TextViewLines;
            
            var span = doc_view.TextView?.Selection.SelectedSpans[0].Span;
            if (span.HasValue)
            {
                var replace_with = "test line 1\ntest line 2\n test line 3\n";
                doc_view.TextBuffer.Replace(span.Value, replace_with);
            }
        }
    }
}
