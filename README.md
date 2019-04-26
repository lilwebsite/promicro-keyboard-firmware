# promicro-keyboard-firmware
Keyboard firmware written in C using avr-libc, designed for the pro micro MCU (ATmega32U4)

# What this is

![IBMPingmaster keyboard](https://bigwebsite.cool/content/IBMPingmaster.jpg)
This program is a firmware driver for keyboards, at the moment only supporting the IBM Pingmaster line of keyboards, and is used to make promicro or similar a drop in replacement for the existing microcontroller attached to the keyboard.

This program was written to work with the pingmaster, but there are many keyboards that can work using this firmware so it is possible to use it on most keyboards, with a few modifications. Feel free to use this software for whatever you like, everything is MIT licensed.

I started writing this program since I had many issues with existing methods for converting the Pingmaster (such as converting or using a drop in replacement) and after trying many different methods for getting the pro micro to work with the Pingmaster I decided to write my own firmware. This dosen't necessarily mean that this program is the best method for modding the Pingmaster, it is likely easier to use TMK converter and just be done with it, but I wanted to upload this project regardless since I think the information may prove useful to someone. Although I'm not sure if anyone will be using this firmware, I will continue developing / updating it, since I have had quite a bit of fun creating this project. For those of you interested, please do enjoy the firmware I have created, and feel free to add feature requests.

v1.0
only support for IBM Pingmaster
