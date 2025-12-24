# generate_sources.ps1
# Writes a sources.txt file listing every .cpp under ./src with each path quoted.
# Writes UTF-8 without BOM using .NET WriteAllLines.

# Ensure we run in the workspace folder (VS Code tasks set cwd to workspace root)
Set-Location -LiteralPath (Get-Location)

[int]$count = 0
$files = Get-ChildItem -Recurse -Path .\src -Filter *.cpp -File | ForEach-Object {
	$count++
	$p = $_.FullName -replace '\\','/'
	'"' + $p + '"'
}
[Console]::WriteLine("Wrote $count entries to sources.txt")
$files | Out-File -FilePath (Join-Path (Get-Location) 'sources.txt') -Encoding ascii
