# cwxdaemon
A partial implementation of the popular CWDaemon that works with Flex's CWX API

## Instructions

1. Have a g++ compiler that supports the C++ 11 standard. ( -std=c++0x )
2. run 'make'
3. put the cwxdaemon binary somewhere in your PATH.

## Arguments

The program accepts 3 different arguments:

|Command Line Option            | Description |
|:-----------------|:------------|
|--udpport=[port] |the UDP port to listen for cwdaemon commands.|
|--flexhost=[host] |the hostname or IP address assigned to your Flex 6000 series radio.|
|--flexport=[port] |optional. Default is 4992, which is the documented Flex API port. Don't change this unless you REALLY know what you're doing.|
  
## CWDAEMON Commands Supported

This program, and the Flex CWX API, support a small sub-set of the cwdaemon commands. Those are:

|Command Byte | Action |
|-------------|--------|
|[esc]2 [wpm] | sets the WPM to the integer immediately following the command by invoking the "cwx wpm" command.|
|[esc]4 | stops sending CW immediately by invoking the "cwx clear" command.|
|[esc]5 | exits the cwxdaemon program.|
| [bunch of text] | sends [bunch of text] as a CW message using the "cwx send" command.|

Note that cwdaemon commands are distinguished from CW messages by the ascii ESC character as the first byte of the message. Your cwdaemon-compatible software should handle this. See the cwdaemon documentation for further details.

Prosigns are interpreted by the CWX API and may vary from those specified by cwdaemon. I was too lazy to translate these.
* To insert Prosign BT use =
* To insert Prosign AR use +
* To insert Prosign KN use (
* To insert Prosign BK use &
* To insert Prosign SK use $

## Example invocation

This program is meant to run in the background. There are some error messages, and if you don't want to see them, you should redeirect output to /dev/null. In the case where your Flex 6000 series radio is assigned the IP address 192.168.1.100, do this:

```$ cwxdaemon --udpport=60001 --flexhost=192.168.1.100 &```

In the case where you've given your Flex 6000 series radio the host name 'flex6000' and you want to redirect the program's output to /dev/null, do this

```$  cwxdaemon --udpport=60001 --flexhost=flex6000 >/dev/null 2>&1 &```

Then, configure your cwdaemon-compatible program to look for cwdaemon on port 60001 and you should be ready to go.
