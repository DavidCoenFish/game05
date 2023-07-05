using Community.VisualStudio.Toolkit;

using Microsoft.VisualStudio.Shell;

using System;
using System.Runtime.InteropServices;
using System.Threading;

using Task = System.Threading.Tasks.Task;

namespace dsc_vsix
{
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
    [InstalledProductRegistration(Vsix._name, Vsix._description, Vsix._version)]
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [Guid(PackageGuids._main_guid_string)]
    public sealed class DscVsixPackage : ToolkitPackage
    {
        protected override async Task InitializeAsync(CancellationToken cancellationToken, IProgress<ServiceProgressData> progress)
        {
            await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
            await CommandsOrderIncludes.InitializeAsync(this);
        }
    }
}