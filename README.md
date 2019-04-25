# promicro-keyboard-firmware
Keyboard firmware written in C using avr-libc, designed for the promicro MCU

# What this is
This program is a firmware driver for keyboards, at the moment only supporting the IBM Pingmaster line of keyboards. This program is used to make any promicro MCU a drop in replacement for the existing MCU(microcontroller) attached to the keyboard. The reason I wrote this program was to be able to use the pingmaster with any layout or key configuration, and to add special macro keys and such to make it generally more useful. There are already quite a few ways to do this with existing firmware and converters, but there were a lot of issues I had when using those methods, specifically with the promicro MCU, so in turn I decided to just write my own firmware. This program was written to work with the pingmaster exclusively, but there are many keyboards that can work using this firmware so it is possible to use it on most keyboards, with a few modifications.

v1.0
only support for IBM Pingmaster
