# Valid extensions
$validExtensions = @{
  ".vert" = $true
  ".tesc" = $true
  ".tese" = $true
  ".geom" = $true
  ".frag" = $true
  ".comp" = $true
  ".mesh" = $true
  ".task" = $true
  ".rgen" = $true
  ".rint" = $true
  ".rahit" = $true
  ".rchit" = $true
  ".rmiss" = $true
  ".rcall" = $true
}
# Get filenames in current directory
$files = Get-ChildItem

for ($i=0; $i -lt $files.Count; $i++) {
  $infile = $files[$i].FullName
  $extension = [IO.Path]::GetExtension($infile)

  if ($validExtensions.ContainsKey($extension)) {
    $outfile = $infile + ".spv"
    $command = "glslangValidator -V $infile -o $outfile"
    Invoke-Expression "& $command"
  }
}
