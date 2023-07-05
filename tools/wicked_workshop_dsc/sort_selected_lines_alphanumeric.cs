using Community.VisualStudio.Toolkit;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.ComponentModel.Design;
using System.Globalization;
using System.Threading;
using System.Threading.Tasks;
using Task = System.Threading.Tasks.Task;

namespace wicked_workshop_dsc
{

    /// <summary>
    /// Command handler
    /// </summary>
    internal sealed class sort_selected_lines_alphanumeric
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        public const int CommandId = 0x0100;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("ac8f8769-1040-45b1-bb94-cb604ece0fbd");

        /// <summary>
        /// VS Package that provides this command, not null.
        /// </summary>
        private readonly AsyncPackage package;

        /// <summary>
        /// Initializes a new instance of the <see cref="sort_selected_lines_alphanumeric"/> class.
        /// Adds our command handlers for menu (commands must exist in the command table file)
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        /// <param name="commandService">Command service to add command to, not null.</param>
        private sort_selected_lines_alphanumeric(AsyncPackage package, OleMenuCommandService commandService)
        {
            this.package = package ?? throw new ArgumentNullException(nameof(package));
            commandService = commandService ?? throw new ArgumentNullException(nameof(commandService));

            var menuCommandID = new CommandID(CommandSet, CommandId);
            var menuItem = new MenuCommand(this.Execute, menuCommandID);
            commandService.AddCommand(menuItem);
        }

        /// <summary>
        /// Gets the instance of the command.
        /// </summary>
        public static sort_selected_lines_alphanumeric Instance
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the service provider from the owner package.
        /// </summary>
        private Microsoft.VisualStudio.Shell.IAsyncServiceProvider ServiceProvider
        {
            get
            {
                return this.package;
            }
        }

        /// <summary>
        /// Initializes the singleton instance of the command.
        /// </summary>
        /// <param name="package">Owner package, not null.</param>
        public static async Task InitializeAsync(AsyncPackage package)
        {
            // Switch to the main thread - the call to AddCommand in sort_selected_lines_alphanumeric's constructor requires
            // the UI thread.
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync(package.DisposalToken);

            OleMenuCommandService commandService = await package.GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
            Instance = new sort_selected_lines_alphanumeric(package, commandService);
        }


        public async Task<int> MethodAsync() 
        {
            return await Task.Run(() => { 
                string title = "sort_selected_lines_alphanumeric 01";
            
                DocumentView doc_view = await VS.Documents.GetActiveDocumentViewAsync();
                var lines = doc_view.TextView?.Selection.TextView.TextViewLines;
                string message = "";
                foreach(var line in lines)
                {
                    message += line;
                }

                var span = doc_view.TextView?.Selection.SelectedSpans[0].Span;
                if (span.HasValue)
                {
                    var replace_with = "test line 1\ntest line 2\n test line 3\n";
                    doc_view.TextBuffer.Replace(span.Value, replace_with);
                }

                VsShellUtilities.ShowMessageBox(
                    this.package,
                    message,
                    title,
                    OLEMSGICON.OLEMSGICON_INFO,
                    OLEMSGBUTTON.OLEMSGBUTTON_OK,
                    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
            
            
            });
        }

        //protected override async Task ExecuteAsync(OleMenuCmdEventArgs e)
        //{
        private void Execute(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            string title = "sort_selected_lines_alphanumeric 01";
            
            //string message = string.Format(CultureInfo.CurrentCulture, "Inside {0}.MenuItemCallback()", this.GetType().FullName);
            

            var task = MethodAsync();

            //Community.VisualStudio.Toolkit.VS.Documents.

            //DocumentView doc_view = VS.Documents.GetActiveDocumentViewAsync();
            //var lines = doc_view.TextView?.Selection.TextView.TextViewLines;
            //string message = "";
            //foreach(var line in lines)
            //{
            //    message += line;
            //}

            //var span = doc_view.TextView?.Selection.SelectedSpans[0].Span;
            //if (span.HasValue)
            //{
            //    var replace_with = "test line 1\ntest line 2\n test line 3\n";
            //    doc_view.TextBuffer.Replace(span.Value, replace_with);
            //}


            // Show a message box to prove we were here
            //VsShellUtilities.ShowMessageBox(
            //    this.package,
            //    message,
            //    title,
            //    OLEMSGICON.OLEMSGICON_INFO,
            //    OLEMSGBUTTON.OLEMSGBUTTON_OK,
            //    OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
        }
    }
    /**/
    /*

    [Command(0x0100)]
    internal sealed class sort_selected_lines_alphanumeric : BaseCommand<sort_selected_lines_alphanumeric>
    {
        /// <summary>
        /// Command ID.
        /// </summary>
        public const int CommandId = 0x0100;

        /// <summary>
        /// Command menu group (command set GUID).
        /// </summary>
        public static readonly Guid CommandSet = new Guid("ac8f8769-1040-45b1-bb94-cb604ece0fbd");


        protected override async Task ExecuteAsync(OleMenuCmdEventArgs e)
        {
      //    var document_window_list = await VS.Windows.GetAllDocumentWindowsAsync();
            DocumentView doc_view = await VS.Documents.GetActiveDocumentViewAsync();
            var lines = doc_view.TextView?.Selection.TextView.TextViewLines;

            string message = "";
            foreach(var line in lines)
            {
                message += line;
            }

            var span = doc_view.TextView?.Selection.SelectedSpans[0].Span;
            if (span.HasValue)
            {
                var replace_with = "test line 1\ntest line 2\n test line 3\n";
                doc_view.TextBuffer.Replace(span.Value, replace_with);
            }

            string title = "sort_selected_lines_alphanumeric 02";

            VsShellUtilities.ShowMessageBox(
                Package,
                message,
                title,
                OLEMSGICON.OLEMSGICON_INFO,
                OLEMSGBUTTON.OLEMSGBUTTON_OK,
                OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
        }
    }
*/
}
