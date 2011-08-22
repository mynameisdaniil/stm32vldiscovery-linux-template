#!/bin/sh

if [ ! -e /etc/udev/rules.d/10-stlink.rules ]; then
    echo "stlink udev rule not found in folder with \
udev rules!"
    echo "Please take ./stlink/10-stlink.rules and \
put it to corresponding folder."
    echo "After that execute: 'udevadm control --reload-rules'"
    exit 1
fi

if [ ! -e /etc/modprobe.d/stlink.modprobe.conf ]; then
    echo "stlink.modprobe.conf not found in /etc/modprobe.d!"
    echo "Please take ./stlink/stlink.modprobe.conf and put \
it to corresponding folder."
    exit 1
fi

# Check - is sg module loaded. If not load it. If error - exit.
lsmod | grep -q sg
if [ "$?" -ne "0" ]; then
    echo "sg module not loaded - loading it..."
    sudo modprobe sg
    if [ "$?" -ne "0" ]; then
	echo "Error loading module sg!"
	echo "Try to recompile you kernel with \
option 'CONFIG_CHR_DEV_SG=m'."
	echo "You may find it in 'Device Drivers' -> \
'SCSI' drivers"
	exit 1
    fi
fi

make -C ./stlink/build
if [ "$?" -ne "0" ]; then
    echo "May be you do not installed libsgutils2 \
development files?"
    echo "For Ubuntu users: try 'sudo apt-get-install libsgutils2-dev'"
    echo "For Gentoo users: try 'sudo emerge sg3-utils'"
    exit 1
fi

echo "*****************************************************************"
echo "*  Starting GDB server...                                       *"
echo "*  For connect to it - run GDB in another console and:          *"
echo "*   (gdb) target remote :1234                                   *"
echo "*  For load ELF to STM32VLDiscovery                             *"
echo "*   (gdb) load $1                                               *"
echo "*****************************************************************"

sudo ./stlink/build/st-util 1234 /dev/stlink
