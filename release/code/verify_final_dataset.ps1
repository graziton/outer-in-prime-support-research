$ErrorActionPreference = "Stop"

$codeDir = $PSScriptRoot
$src = Join-Path $codeDir "verify_solutions.cpp"
$exe = Join-Path $codeDir "verify_solutions.exe"
$input = Join-Path $codeDir "..\verification\final_22_verification_input.txt"
$outFile = Join-Path $codeDir "..\verification\final_22_verification_runtime.csv"

if (-not (Test-Path $src)) { throw "Missing $src" }
if (-not (Test-Path $input)) { throw "Missing $input" }

$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if ($null -eq $gpp) {
    throw "g++ was not found. Install MinGW-w64 or another C++17 compiler."
}

Write-Host "Compiling verifier..."
& $gpp.Source -O3 -std=c++17 $src -o $exe
if ($LASTEXITCODE -ne 0) { throw "Verifier compilation failed." }

Write-Host "Running verifier..."
& $exe $input $outFile
if ($LASTEXITCODE -ne 0) { throw "Verifier execution failed." }

Write-Host "Verification complete. Inspect the summary above." 
