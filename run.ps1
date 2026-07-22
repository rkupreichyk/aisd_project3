<#
Single entry point for this project.

    .\run.ps1                             build, then run interactively
                                           (type input, then Ctrl+Z + Enter for EOF)
    .\run.ps1 -InputFile path\to\file.txt  build, then run against one input file
    .\run.ps1 -Tests                       build + (auto-generate if missing) + run
                                           the full test suite, checking time/memory limits
    .\run.ps1 -Tests -TimeLimitSeconds 10 -MemoryLimitKB 65536
    .\run.ps1 -Tests -Regenerate           force-regenerate tests/*.txt first
    .\run.ps1 -SkipBuild ...               reuse the existing engine.exe
#>

param(
    [switch]$Tests,
    [switch]$Regenerate,
    [switch]$SkipBuild,
    [string]$InputFile,
    [double]$TimeLimitSeconds = 15.0,
    [double]$MemoryLimitKB = 65536.0
)

$ErrorActionPreference = "Stop"
$root = $PSScriptRoot
$exePath = Join-Path $root "engine.exe"

if (-not $SkipBuild) {
    . (Join-Path $root "msvc_env.ps1")
    Import-VsDevEnvironment
    Write-Host "Building..."
    Push-Location $root
    try {
        $sources = "main.cpp", "graph.cpp", "vertex.cpp", "node.cpp", "list.cpp", "unionfind.cpp", "degreesequence.cpp"
        & cl.exe /nologo /O2 /EHsc /std:c++17 "/Fe:$exePath" @sources 2>&1 | Out-Null
        if (-not (Test-Path $exePath)) {
            throw "Build failed -- exe not produced. Run cl.exe manually to see the errors."
        }
    }
    finally {
        Pop-Location
    }
    Write-Host "Built $exePath"
    Write-Host ""
}
elseif (-not (Test-Path $exePath)) {
    throw "No existing exe at $exePath -- run without -SkipBuild first."
}

if ($Tests) {
    $benchArgs = @{
        SkipBuild        = $true
        TimeLimitSeconds = $TimeLimitSeconds
        MemoryLimitKB    = $MemoryLimitKB
    }
    if ($Regenerate) { $benchArgs["Regenerate"] = $true }
    & (Join-Path $root "tests\run_benchmarks.ps1") @benchArgs
    exit $LASTEXITCODE
}

if ($InputFile) {
    if (-not (Test-Path $InputFile)) { throw "Input file not found: $InputFile" }
    # Use real file-handle redirection (Start-Process), not a PowerShell pipeline --
    # piping stdin through `Get-Content | & $exe` silently delivers empty input to
    # the child process in this environment (the child sees immediate EOF).
    $tmpOut = [System.IO.Path]::GetTempFileName()
    try {
        $proc = Start-Process -FilePath $exePath -RedirectStandardInput $InputFile `
            -RedirectStandardOutput $tmpOut -NoNewWindow -PassThru
        [void]$proc.Handle
        $proc.WaitForExit()
        Get-Content $tmpOut
        if ($proc.ExitCode -ne 0) {
            Write-Host "Process exited with code $($proc.ExitCode)" -ForegroundColor Yellow
        }
    }
    finally {
        Remove-Item $tmpOut -ErrorAction SilentlyContinue
    }
}
else {
    Write-Host "Running interactively -- type input, then Ctrl+Z and Enter (Windows EOF) when done."
    & $exePath
}
