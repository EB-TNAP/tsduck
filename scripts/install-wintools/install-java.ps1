﻿#-----------------------------------------------------------------------------
#
#  Copyright (c) 2022, Thierry Lelegard
#  BSD-2-Clause license, see the LICENSE.txt file
#
#  Download and install Java for Windows (AdoptOpenJDK community project).
#  See parameters documentation in install-common.ps1.
#
#  Additional parameters:
#
#  -JRE
#     Download and install the JRE. By default, the JDK is installed.
#
#-----------------------------------------------------------------------------


[CmdletBinding(SupportsShouldProcess=$true)]
param(
    [string]$Destination = "",
    [switch]$ForceDownload = $false,
    [switch]$GitHubActions = $false,
    [switch]$NoInstall = $false,
    [switch]$NoPause = $false,
    # Addition parameters:
    [switch]$JRE = $false
)

Write-Output "==== Java (AdoptOpenJDK) download and installation procedure"

. "$PSScriptRoot\install-common.ps1"

# REST API for the latest releases of Eclipse Temurin.
$API = "https://api.adoptium.net/v3"

# Get JDK or JRE.
if ($JRE) {
    $ImageType = "jre"
}
else {
    $ImageType = "jdk"
}

# Get latest LTS version.
$lts = (Invoke-RestMethod $API/info/available_releases).most_recent_lts

# Get a list of assets for this LTS. Extract the one for 64-bit Windows.
$bin = (Invoke-RestMethod $API/assets/latest/$lts/hotspot).binary | `
    Where-Object os -eq windows | `
    Where-Object architecture -eq x64 | `
    Where-Object jvm_impl -eq hotspot | `
    Where-Object heap_size -eq normal | `
    Where-Object image_type -eq $ImageType | `
    Where-Object project -eq jdk | `
    Select-Object -First 1 -Property installer

$InstallerURL = $bin.installer.link
$InstallerName = $bin.installer.name
$InstallerPath = "$Destination\$InstallerName"

if (-not $InstallerURL -or -not $InstallerName) {
    Exit-Script "Cannot find URL for installer"
}

Download-Package $InstallerURL $InstallerPath

if (-not $NoInstall) {
    Write-Output "Installing $InstallerName"
    Start-Process -Verb runas -FilePath msiexec.exe -ArgumentList @("/i", $InstallerPath, "INSTALLLEVEL=3", "/quiet", "/qn", "/norestart") -Wait
}

Propagate-Environment "JAVA_HOME"
Exit-Script
