# generate_sources.ps1
# Writes a sources.txt file listing every .cpp and .c under ./src with each path quoted.
# Writes UTF-8 without BOM.

Set-Location -LiteralPath (Get-Location)

$srcRoot = Join-Path (Get-Location) 'src'
$outPath = Join-Path (Get-Location) 'sources.txt'

# Collect .cpp and .c files
$files = Get-ChildItem -Recurse -Path $srcRoot -File |
    Where-Object { $_.Extension -in @('.cpp', '.c') } |
    ForEach-Object {
        $p = $_.FullName -replace '\\','/'
        '"' + $p + '"'
    }

[int]$count = $files.Count
[Console]::WriteLine("Wrote $count entries to sources.txt")

# UTF-8 without BOM
$utf8NoBom = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllLines($outPath, $files, $utf8NoBom)
