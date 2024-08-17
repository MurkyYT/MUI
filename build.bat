@echo off

if "%~1"=="" goto blank

if "%~1"=="-release" goto release else goto blank

:RELEASE
    msbuild  /property:Configuration=Release
    goto end

:BLANK
    msbuild
    goto end

:END
