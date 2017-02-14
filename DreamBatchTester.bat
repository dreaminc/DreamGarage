@echo off

REM set /p listUsernames=Enter username:
REM set /p password=Enter Password:
REM set /p env=Enter Environment ID:

set username=jason_test1@dreamos.com
set password=nightmare
set env=15

set dreamPathExe=DreamOS_Release.lnk -u %username% -p %password% -env %env%
echo %dreamPathExe%
start "%username%" %dreamPathExe%

set username=jason_test2@dreamos.com
set password=nightmare
set env=15

set dreamPathExe=DreamOS_Release.lnk -u %username% -p %password% -env %env%
echo %dreamPathExe%
start "%username%" %dreamPathExe%




