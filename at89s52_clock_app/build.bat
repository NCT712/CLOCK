@echo off
rem ==============================================================================
rem BUILD SCRIPT FOR AT89S52 CLOCK APP (WINDOWS)
rem ==============================================================================

setlocal enabledelayedexpansion

echo =====================================================================
echo  BUILDING AT89S52 DIGITAL CLOCK APP (SDCC + AVRDUDE)
echo =====================================================================

if not exist build mkdir build
if not exist build\Driver mkdir build\Driver
if not exist build\Module mkdir build\Module
if not exist build\UserAPP mkdir build\UserAPP

set CFLAGS=-mmcs51 --model-small --std-c99 --opt-code-speed
set INCLUDES=-ISource\Driver -ISource\Module -ISource\UserAPP

echo [1/8] Compiling Driver/Timer0.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Driver\Timer0.c -o build\Driver\Timer0.rel
if errorlevel 1 goto error

echo [2/8] Compiling Driver/GPIO.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Driver\GPIO.c -o build\Driver\GPIO.rel
if errorlevel 1 goto error

echo [3/8] Compiling Module/Segment.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Module\Segment.c -o build\Module\Segment.rel
if errorlevel 1 goto error

echo [4/8] Compiling Module/Button.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Module\Button.c -o build\Module\Button.rel
if errorlevel 1 goto error

echo [5/8] Compiling Module/Buzzer.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Module\Buzzer.c -o build\Module\Buzzer.rel
if errorlevel 1 goto error

echo [6/8] Compiling Module/Led.c...
sdcc %CFLAGS% %INCLUDES% -c Source\Module\Led.c -o build\Module\Led.rel
if errorlevel 1 goto error

echo [7/8] Compiling UserAPP/clock_app.c...
sdcc %CFLAGS% %INCLUDES% -c Source\UserAPP\clock_app.c -o build\UserAPP\clock_app.rel
if errorlevel 1 goto error

echo [8/8] Compiling UserAPP/main.c...
sdcc %CFLAGS% %INCLUDES% -c Source\UserAPP\main.c -o build\UserAPP\main.rel
if errorlevel 1 goto error

echo [LINK] Linking objects into build\clock_app.ihx...
sdcc %CFLAGS% build\Driver\Timer0.rel build\Driver\GPIO.rel build\Module\Segment.rel build\Module\Button.rel build\Module\Buzzer.rel build\Module\Led.rel build\UserAPP\clock_app.rel build\UserAPP\main.rel -o build\clock_app.ihx
if errorlevel 1 goto error

echo [PACKIHX] Generating Intel HEX file...
packihx build\clock_app.ihx > build\clock_app.hex
if errorlevel 1 goto error

echo.
echo =====================================================================
echo  BUILD SUCCESSFUL: build\clock_app.hex
echo =====================================================================
echo.
echo De nap code, hay cam mach nap USBasp va chay:
echo   avrdude -C ..\avrdude.conf -c usbasp -p 89s52 -U flash:w:build\clock_app.hex:i
echo.
goto end

:error
echo.
echo *********************************************************************
echo  BUILD FAILED! Vui long kiem tra loi bien dich o tren.
echo *********************************************************************
exit /b 1

:end
