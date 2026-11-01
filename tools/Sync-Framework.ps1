param([ValidateRange(15,22)][int[]]$Practice = @(15,16,17,18,19), [switch]$Standard)
$ErrorActionPreference = 'Stop'
$repository = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$source = Join-Path $repository 'framework'
$projects = @($Practice | ForEach-Object { "openGL_3dAssign_$_" })
if ($Standard) { $projects += 'standard' }
foreach ($name in $projects) {
    $project = Join-Path $repository $name
    if (-not (Test-Path -LiteralPath $project)) { continue }
    $target = [IO.Path]::GetFullPath((Join-Path $project 'framework'))
    if (-not $target.StartsWith($repository + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "Target leaves repository: $target" }
    if ((Get-Item -LiteralPath $project).Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Refuse linked project: $project" }
    if ((Test-Path -LiteralPath $target) -and ((Get-Item -LiteralPath $target).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw "Refuse linked target: $target" }
    foreach ($file in Get-ChildItem -LiteralPath $source -Recurse -File) {
        if ($file.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Refuse linked source: $($file.FullName)" }
        $relative = $file.FullName.Substring($source.Length + 1)
        $destination = Join-Path $target $relative
        New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
    }
    Write-Output "Synchronized $name (overlay; no recursive deletion)"
}
