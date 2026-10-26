@echo off
setlocal EnableDelayedExpansion

echo =============================================
echo    File Version List - Current Folder
echo =============================================
echo.

echo File Name^|Version
echo --------------------

cd /d "C:\_dev\FabV25\bin\release"

for %%F in (*.exe *.dll) do (
    set "ver="
    for /f "skip=1 tokens=*" %%V in ('wmic datafile where name^="%CD:\=\\%\\%%F" get version 2^>nul') do (
        if not defined ver set "ver=%%V"
    )
    if defined ver (
        echo %%F ^| !ver!
    ) else (
        echo %%F ^| (no version info)
    )
)

