<#
Dot-sourced helper: makes sure cl.exe is usable in the current PowerShell
session. Plain PowerShell/cmd windows don't have the MSVC compiler on PATH
-- that only happens inside a "Developer Command Prompt/PowerShell for VS",
which normally means launching a special shortcut from the Start menu
every time. This finds the Visual Studio install via vswhere and loads
the same environment vcvarsall.bat would set up, directly into the
current process, so a plain terminal works too.
#>

function Import-VsDevEnvironment {
    if (Get-Command cl.exe -ErrorAction SilentlyContinue) {
        return
    }

    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        throw "cl.exe is not on PATH and vswhere.exe was not found. Install the 'Desktop development with C++' workload via the Visual Studio Installer, or run this from a 'Developer PowerShell for VS' prompt."
    }

    $vsInstallPath = & $vswhere -latest -products * -property installationPath
    if (-not $vsInstallPath) {
        throw "vswhere.exe did not find any Visual Studio installation. Install the 'Desktop development with C++' workload via the Visual Studio Installer."
    }

    $vcvarsall = Join-Path $vsInstallPath "VC\Auxiliary\Build\vcvarsall.bat"
    if (-not (Test-Path $vcvarsall)) {
        throw "Found a Visual Studio install at '$vsInstallPath' but no VC\Auxiliary\Build\vcvarsall.bat -- the C++ build tools workload may not be installed."
    }

    Write-Host "cl.exe not on PATH -- loading the MSVC build environment from:`n  $vcvarsall"

    $envDump = & cmd.exe /c "`"$vcvarsall`" x64 2>nul && set"
    foreach ($line in $envDump) {
        if ($line -match '^([^=]+)=(.*)$') {
            Set-Item -Path "Env:$($matches[1])" -Value $matches[2] -Force
        }
    }

    if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
        throw "Loaded vcvarsall.bat but cl.exe still isn't on PATH -- something unexpected about this VS install."
    }
}
