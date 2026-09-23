$ErrorActionPreference = "Stop"

$src = Join-Path $PSScriptRoot "exhaustive_search.cpp"
$exe = Join-Path $PSScriptRoot "exhaustive_search.exe"

if (-not (Test-Path $src)) {
    throw "Missing source file: $src"
}

if (-not (Test-Path $exe)) {
    $gpp = Get-Command g++ -ErrorAction SilentlyContinue
    if ($null -eq $gpp) {
        throw "g++ was not found. Install a C++17 compiler such as MinGW-w64, then rerun this script."
    }

    Write-Host "Compiling exhaustive_search.cpp ..."
    & $gpp.Source -O3 -std=c++17 -pthread $src -o $exe

    if ($LASTEXITCODE -ne 0) {
        throw "Compilation failed."
    }
}

$threads = 8
$chunk   = [int64]2000000
$rawDir  = Join-Path $PSScriptRoot "raw"

if (Test-Path $rawDir) {
    Remove-Item -Recurse -Force $rawDir
}
New-Item -ItemType Directory -Path $rawDir | Out-Null

$rawFiles = @()

# One complete decimal digit length per invocation.
# This keeps the digit-position permutation fixed during each run.
for ($digits = 1; $digits -le 10; ++$digits) {
    [int64]$lo = 1
    for ($i = 1; $i -lt $digits; ++$i) {
        $lo *= 10
    }
    [int64]$hi = $lo * 10

    $file = Join-Path $rawDir ("hits_{0}digit.csv" -f $digits)
    $rawFiles += $file

    Write-Host "========================================"
    Write-Host "Digit length: $digits"
    Write-Host "Scanning [$lo,$hi)"
    Write-Host "Output: $file"
    Write-Host "========================================"

    & $exe $lo $hi $threads $chunk $file

    if ($LASTEXITCODE -ne 0) {
        throw "Search failed for [$lo,$hi)"
    }
}

$allFile = Join-Path $PSScriptRoot "primitive_hits_all.csv"

# Import-Csv returns no rows for a header-only file, which is fine.
$rows = @(
    foreach ($file in $rawFiles) {
        Import-Csv $file
    }
)

$rows |
    Sort-Object { [int64]$_.n } |
    Export-Csv $allFile -NoTypeInformation

Write-Host ""
Write-Host "========================================"
Write-Host "FULL SEARCH COMPLETE"
Write-Host "Combined output: $allFile"
Write-Host "Hit count: $($rows.Count)"
Write-Host "========================================"
