#!/bin/sh

FLASH_ERR='no'
MAKE_ERR='no'
FLASH_DFU='yes'
NOCLEAN='no'
FLASH_DEVICE='no'
FLASH_BOOTLOADER='no'
COMPILE='no'
DEBUG='no'

_usage ()
{
	echo "usage ${0} [[options]]"
	echo
	echo "running this script without any options will compile the .hex file if"
	echo "it doesn't exist, or if it does eixst, it will print out the binary size"
	echo
	echo "-C		Compile the firmware"
	echo "			 Default: ${COMPILE}"
	echo "-G		Compile the program in debug mode"
	echo "			 Default: ${DEBUG}"
	echo "-N		Compile without 'make clean'"
	echo "			 Default: ${NOCLEAN}"
	echo "-D		Flash with a DFU bootloader instead of an ISP programmer"
	echo "			 Default: ${FLASH_DFU}"
	echo "-F		Flash the device when the build finishes"
	echo "			 Default: ${FLASH_DEVICE}"
	echo "-B		Flash the device with a bootloader (overrides all options, requires ISP programmer)"
	echo "			 Default: ${FLASH_BOOTLOADER}"
	echo "-h		This help message"
	exit ${1}
}

while getopts 'CNDFBGh' arg; do
	case "${arg}" in
		C) COMPILE='yes' ;;
		N) NOCLEAN='yes' ;;
		D) FLASH_DFU='no' ;;
		F) FLASH_DEVICE='yes' ;;
		B) FLASH_BOOTLOADER='yes' ;;
		G) DEBUG='yes' ;;
		h) _usage 0 ;;
		*)
			echo "Invalid argument '${arg}'"
			_usage 1
			;;
	esac
done

if [[ $FLASH_BOOTLOADER = 'yes' ]]
then
	make flash-bootloader FLASH_DFU='no'
	exit $?
fi

if [[ $NOCLEAN = 'no' && $COMPILE = 'yes' || ! -f 'kbd.hex' ]]
then
	make fullclean
fi

export DEBUG
if [[ ! -f 'kbd.hex' ]]; then make; fi
if [[ ! $? == 0 ]]; then MAKE_ERR='yes'; fi

if [[ $MAKE_ERR = 'no' && $FLASH_DEVICE = 'yes' ]]
then
	export FLASH_DFU
	if [[ $FLASH_DFU == 'yes' ]]
	then
		HICCUP='no'
		FLASH_ERR='yes'
		echo
		echo "Flash will begin in a moment, press the reset switch now."
		echo
		sleep 1.5s
		for (( x=1; x<=4; x++ ))
		do
			sleep 0.5s
			make -e erasechip
			if [[ ! $? == 0 ]]; then HICCUP='yes'; continue; fi
			make -e flash
			if [[ ! $? == 0 ]]; then HICCUP='yes'; continue; fi
			FLASH_ERR='no'
			break
		done
		if [[ $FLASH_ERR = 'no' ]]; then make launch; fi
	else
		make -e erasechip
		make -e flash
		if [[ ! $? == 0 && $MAKE_ERR = 'no' ]]; then FLASH_ERR='yes'; fi
		make -e validate
	fi
fi

export FLASH_DEVICE
export FLASH_ERR
export MAKE_ERR
export HICCUP
make -e finish
