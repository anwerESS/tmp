@echo off
setlocal enabledelayedexpansion

if "%1"=="" (
    echo Usage: treelevel.bat [directory] [depth]
    goto :eof
)

set "dir=%1"
set /a maxdepth=%2

call :recurse "%dir%" 1

goto :eof

:recurse
set "path=%1"
set /a depth=%2

if %depth% leq %maxdepth% (
    for /d %%d in ("%path%\*") do (
        for /l %%i in (1,1,%depth%) do set "tab=!tab!    "
        echo !tab!|findstr /r /c:"    " 1>nul && echo ^|!tab!----%%~nxd
        set "tab="
        call :recurse "%%d" !depth!+1
    )
)

exit /b
