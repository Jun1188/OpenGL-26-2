param([ValidateRange(13,19)][int[]]$Practice = @(13,14,15,16,17,18,19))
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$source = Join-Path $repository 'framework'
foreach ($number in $Practice) {
    $project = Join-Path $repository "openGL_3dAssign_$number"
    if (-not (Test-Path -LiteralPath $project)) { continue }
    $target = [IO.Path]::GetFullPath((Join-Path $project 'framework'))
    if (-not $target.StartsWith($repository + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Target leaves repository: $target"
    }
    if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Recurse -Force }
    Copy-Item -LiteralPath $source -Destination $target -Recurse
    Write-Output "Synchronized practice $number"
}
