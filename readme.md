LIBHD44780
==========

LIBHD44780 is an AVR library for controlling the HD44780 family of LCDs.

Table of Contents
===============

1. [Changelog](https://github.com/majestic53/libhd44780#changelog)
2. [Usage](https://github.com/majestic53/libhd44780#usage)
3. [License](https://github.com/majestic53/libhd44780#license)

Changelog
=========

###Version 0.1.1509
*Updated: 12/23/2015*

* Code cleanup (overhaul)

*Updated: 12/22/2015*

* Repo creation
* Added USART tracing functionality

Usage
=====

**NOTE:** You will need to install avr-gcc if you don't already have it.

To use LIBHD44780, simply include the header file ```hd44780.h```, found under ```./src/lib/include```, in your project.

To use the sample project, compile the core library using the accompanying makefile. The makefile will produce a series of 
object files (found in the ```./build``` directory), and a series of hex files (found in the ```./bin``` directory).

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
