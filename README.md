## win-xkill
A porting of the xkill utility for Windows.

---------------------------------------------------------------------------------------------------------

xkill is a utility program distributed with the X Window System that instructs the X server to
forcefully terminate its connection to a client, thus "killing" the client.

Even if this program is very dangerous, it can be really useful for aborting programs that have displayed undesired windows on a user's screen.

For this reason, some years ago I decided to port it to Windows. After all this time, I decided to refactor
and improve the code to make it public again.

---------------------------------------------------------------------------------------------------------

_Note that most of the code was taken from the [original source code](https://opensource.apple.com/source/X11apps/X11apps-13/xkill/xkill-X11R7.0-1.0.1/xkill.c),
I just took it, ported the non-portable code to Windows and turned the arguments' ```-```s into ```/```s._

---------------------------------------------------------------------------------------------------------

### Usage

```
xkill [/option ...]
where options include:
    /id resource            resource whose client is to be killed
    /button number          specific button to be pressed to select window
```

Just as the original xkill, you can close a process by clicking it or by passing its PID as an argument, and you can also choose the button to use to kill the process.
