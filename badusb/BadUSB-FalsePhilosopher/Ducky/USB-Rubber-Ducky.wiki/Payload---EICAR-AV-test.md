A simple test to place the EICAR AV test string in a file on the Desktop, which in theory should trigger the AV on the host. Useful if you're assessing a host with no other connectivity.

Original version by tautology.

```
DEFAULTDELAY 500
GUI r
STRING notepad.exe
ENTER
STRING X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*
CTRL s
STRING %USERPROFILE%\desktop\eicar.com
ENTER
ALT F4
```