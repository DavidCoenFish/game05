#as admin in powershell
# Set-ExecutionPolicy RemoteSigned -Scope CurrentUser

$destination=$args[0]
$source=$args[1]

$replace = $false

If (Test-Path $source)
{
  If (Test-Path $destination)
  {
	$replace = (Get-FileHash $destination).hash -ne (Get-FileHash $source).hash
  }
  else
  {
	$replace = $true
  }
}

if ($replace)
{
  Write-Host("replace [$destination] with [$source]")
  Remove-Item $destination
  Move-Item -Path $source -Destination $destination
}

If (Test-Path $source)
{
  Remove-Item $source
}