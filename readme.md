LIBHD44780
==========

LIBHD44780 is an AVR library for controlling the HD44780 family of LCDs.

![Hello Github](https://dl.dropboxusercontent.com/u/6410544/hd44780/hello.jpg)

###Supported Features

* Compliant with most HD44780 panels (tested with a LCD and OLED panel)
* Supports both 4 and 8-bit command modes
* Supports a variety of panel dimensions: 16x1, 16x2, 16x4, 20x2, 20x4, 40x2
* Additional panel dimensions can be added as needed. See the [Examples](https://github.com/majestic53/libhd44780#examples) section below for more information.

###Features NOT Supported

* No support for commands that read data (with the exception of the busy command)

Table of Contents
===============

1. [Changelog](https://github.com/majestic53/libhd44780#changelog)
2. [Usage](https://github.com/majestic53/libhd44780#usage)
	* [Build](https://github.com/majestic53/libhd44780#build)
	* [Concepts](https://github.com/majestic53/libhd44780#concepts)
	* [Examples](https://github.com/majestic53/libhd44780#examples)
		* [Basic Usage](https://github.com/majestic53/libhd44780#basic-usage)
		* [Adding Custom Panel Dimensions](https://github.com/majestic53/libhd44780#adding-custom-panel-dimensions)
3. [License](https://github.com/majestic53/libhd44780#license)

Changelog
=========

###Version 0.1.1509
*Updated: 12/28/2015*

* Added 4-bit support
* Added panel dimension support
* Added comments/documentation

*Updated: 12/26/2015*

* More code refactoring
* Removed tracing/error handling
* Added 4-bit support (partial)

*Updated: 12/25/2015*

* Code refactoring

*Updated: 12/24/2015*

* Added init/uninit commands (tested on a 16x2 LCD and OLED display, cold/warm boot)

*Updated: 12/23/2015*

* Code cleanup (overhaul)

*Updated: 12/22/2015*

* Repo creation
* Added USART tracing functionality

Usage
=====

###Build

**NOTE:** You will need to install avr-gcc if you don't already have it.

To use LIBHD44780 in your own project, simply include the header file ```hd44780.h``` and c file ```hd44780.c```, found under ```./src/lib/include``` and ```./src/lib/src```, in your project.

To build the sample project, compile using the accompanying makefile. The makefile will produce a series of 
object files (found in the ```./build``` directory), and a series of binary files (found in the ```./bin``` directory).

###Concepts

###Examples

####Basic Usage

####Adding Custom Panel Dimensions

License
======

Copyright(C) 2015 David Jolly <majestic53@gmail.com>

libhd44780 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libhd44780 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
