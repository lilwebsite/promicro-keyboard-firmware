#!/bin/bash

FLASH_ERR='no'
MAKE_ERR='no'

if [ ! $FLASH_DFU ]
then
	FLASH_DFU='yes'
fi

if [ ! $NOCLEAN ]
then
	NOCLEAN='no'
fi

if [ ! $FLASH_DEVICE ]
then
	FLASH_DEVICE='no'
fi

if [ ! $FLASH_BOOTLOADER ]
then
	FLASH_BOOTLOADER='no'
fi

_usage ()
{
	echo "usage ${0} [options]"
	echo
	echo "-N		Run without 'make clean'"
	echo "			 Default: ${NOCLEAN}"
	echo "-D		Flash with a DFU bootloader instead of an ISP programmer"
	echo "			 Default: ${FLASH_DFU}"
	echo "-F		Flash the device when the build finishes"
	echo "			 Default: ${FLASH_DEVICE}"
	echo "-B		Flash the device with a bootloader (overrides all options, requires ISP programmer)"
	echo "			 Default: ${FLASH_BOOTLOADER}"
	echo "-G		Compile the program in debug mode"
	echo "			 Default: no"
	echo "-h		This help message"
	exit ${1}
}

while getopts 'NDFBGh' arg; do
	case "${arg}" in
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

if [ $FLASH_BOOTLOADER = 'yes' ]
then
	make flash-bootloader FLASH_DFU='no'
	exit $?
fi

if [ $NOCLEAN = 'no' ]
then
	make fullclean
fi

export DEBUG
make
if [ ! $? -eq 0 ]; then MAKE_ERR='yes'; fi

if [ $MAKE_ERR = 'no' -a $FLASH_DEVICE = 'yes' ]
then
	export FLASH_DFU
	if [ $FLASH_DFU = 'yes' ]
	then
		HICCUP='no'
		FLASH_ERR='yes'
		for (( x=1; x<=4; x++ ))
		do
			sleep 0.5s
			make -e erasechip
			if [ ! $? -eq 0 ]; then HICCUP='yes'; continue; fi
			make -e flash
			if [ ! $? -eq 0 ]; then HICCUP='yes'; continue; fi
			FLASH_ERR='no'
			break
		done
		if [ $FLASH_ERR = 'no' ]; then make launch; fi
	else
		make -e erasechip
		make -e flash
		if [ ! $? -eq 0 -a $MAKE_ERR = 'no' ]; then FLASH_ERR='yes'; fi
		make -e validate
	fi
fi

export FLASH_DEVICE
export FLASH_ERR
export MAKE_ERR
export HICCUP
make -e finish
