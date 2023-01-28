@echo off
setlocal EnableDelayedExpansion

cls

for /f %%i in ('git rev-parse --short HEAD') do set git_sha=%%i
if not exist "bin\" mkdir "bin"
if not exist "bin\obj\" mkdir "bin\obj"

:: Set options that are common for both debug and release builds.

set owen_sources= src/*.c ^
                  src/parsing/*.c ^
                  src/semantics/*.c ^
                  src/semantics/expressions/*.c ^
                  src/semantics/expressions/call/*.c ^
                  src/semantics/statements/*.c ^
                  src/ir/*.c ^
                  src/ir/ast_lowering_pass/*.c ^
                  src/ir/ast_lowering_pass/expressions/*.c ^
                  src/ir/ast_lowering_pass/statements/*.c ^
                  src/x64/*.c
                  
set runner_sources= tests/*.c 

set compiler_defines= /D _CRT_SECURE_NO_WARNINGS ^
                      /D VERSION=\"%git_sha%\" ^
                      /D WIN32_LEAN_AND_MEAN

set compiler_warnings= /W4 ^
                       /WX ^
                       /wd4701 ^
                       /wd4703 ^
                       /wd4706 ^
                       /wd4201

set compiler_options= /std:c17 ^
                      /TC ^
                      /MP ^
                      /Fobin\obj\  ^
                      /diagnostics:caret
                    
set owen_compiler_options= /Febin\owen.exe                    
set runner_compiler_options= /Febin\runner.exe
                      
set linker_options= /incremental:no ^
                    /opt:ref ^
                    shell32.lib ^
                    Shlwapi.lib ^
                    /subsystem:console

:: Set build specific options.
if "%1" == "release" (
    set compiler_options= %compiler_options% ^
                          /O2
) else (
    set compiler_options= %compiler_options% ^
                          /Zi ^
                          /Od ^
                          /fsanitize=address
)

set compile_runner="false"
set compile_owen="false"

if "%2" == "both" (
    set compile_runner="true"
    set compile_owen="true"
)

if "%2" == "" (
    set compile_runner="true"
    set compile_owen="true"
)

if "%2" == "owen" (
    set compile_owen="true"
)

if "%2" == "runner" (
    set compile_runner="true"
)

if %compile_owen%=="true" (
    cl %owen_sources%   %compiler_defines% %compiler_warnings% %compiler_options% %owen_compiler_options%   /link %linker_options%
)

if %compile_runner%=="true" (
    cl %runner_sources% %compiler_defines% %compiler_warnings% %compiler_options% %runner_compiler_options% /link %linker_options%
)
