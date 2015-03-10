LIBHD44780
==========

LIBHD44780 is an AVR library for controlling the HD44780 family of LCDs.

![Hello Github](https://dl.dropboxusercontent.com/u/6410544/hd44780/hello.jpg)

###Supported Features

* Compliant with most HD44780 panels (tested with a LCD and OLED panel)
* Supports both 4 and 8-bit command modes
* Supports a variety of panel dimensions: 16x1, 16x2, 16x4, 20x2, 20x4, 40x2
* Additional panel dimensions can be added as needed. See the 
[Adding Custom Panel Dimensions](https://github.com/majestic53/libhd44780#adding-custom-panel-dimensions) section below for more information.

###Features NOT Supported

* No support for commands that read data from the panel (with the exception of the busy command)

Table of Contents
===============

1. [Changelog](https://github.com/majestic53/libhd44780#changelog)
2. [Usage](https://github.com/majestic53/libhd44780#usage)
	* [Build](https://github.com/majestic53/libhd44780#build)
	* [Concept](https://github.com/majestic53/libhd44780#concepts)
	* [Examples](https://github.com/majestic53/libhd44780#examples)
		* [Basic Usage](https://github.com/majestic53/libhd44780#basic-usage)
		* [Adding Custom Panel Dimensions](https://github.com/majestic53/libhd44780#adding-custom-panel-dimensions)
3. [License](https://github.com/majestic53/libhd44780#license)

Changelog
=========

###Version 0.1.1511
*Updated: 3/9/2015*

* Updated sample code

###Version 0.1.1509
*Updated: 2/28/2015*

* Added 4-bit support
* Added panel dimension support
* Added comments/documentation

*Updated: 2/26/2015*

* More code refactoring
* Removed tracing/error handling
* Added 4-bit support (partial)

*Updated: 2/25/2015*

* Code refactoring

*Updated: 2/24/2015*

* Added init/uninit commands (tested on a 16x2 LCD and OLED display, cold/warm boot)

*Updated: 2/23/2015*

* Code cleanup (overhaul)

*Updated: 2/22/2015*

* Repo creation
* Added USART tracing functionality

Usage
=====

###Build

**NOTE:** You will need to install avr-gcc if you don't already have it.

To use LIBHD44780 in your own project, simply include the header file ```hd44780.h``` and c file ```hd44780.c```, found under ```./src/lib/include``` and ```./src/lib/src```, in your project.

To build the sample project, compile using the accompanying makefile. The makefile will produce a series of 
object files (found in the ```./build``` directory), and a series of binary files (found in the ```./bin``` directory).

###Concept

LIBHD4480 maintains a relatively simple state machine, including the state of the display and cursor. Due to this "statefulness", it is important 
that the the library be initialized prior to use. Use the following routine to do this:

```c
hd44780_initialize(
	<Context>, 
	<Dimension_Type>, 
	<Interface_Type>, 
	<Font_Type>, 
	<Data_Port>, 
	<Control_Port>, 
	<Select_Pin>, 
	<Direction_Pin>, 
	<Enable_Pin>
	);
```

This avoids any undesired/undefined behavior. Once the library has been initialized, any number of display, cursor or 
device manipulations can be performed. After the library has completed all actions, it can then be uninitialized using the following routine:

```c
hd44780_uninitialize(
	<Context>
	);
```

###Examples

####Basic Usage

This example is meant to show a basic usage of LIBHD4480. This example first setups up a UART channel and then takes input from it. This input is
then forwarded to a panel for display.

```c
#include <util/delay.h>
#include "../lib/include/hd44780.h"

#define PIN_CTRL_E 2 // PD2
#define PIN_CTRL_RS 4 // PD4
#define PIN_CTRL_RW 3 // PD3
#define PORT_DATA B // PORTB
#define PORT_CTRL D // PORTD

#define BAUD 9600
#include <util/setbaud.h>

inline void
uart_initialize(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif // USE_2X
	UCSR0B = (_BV(RXEN0) | _BV(TXEN0));
	UCSR0C = (_BV(UCSZ01) | _BV(UCSZ00));
}

inline char 
uart_read(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

int 
main(void)
{
	hdcont_t cont;

	hd44780_initialize(&cont, DIMENSION_16_2, INTERFACE_4_BIT, FONT_EN_JP, 
			PORT_DATA, PORT_CTRL, PIN_CTRL_RS, PIN_CTRL_RW, PIN_CTRL_E);

	uart_initialize();
	hd44780_cursor(&cont, CURSOR_ON, CURSOR_BLINK_ON);

	while(1) {
		hd44780_display_putc(&cont, uart_read());
	}

	hd44780_uninitialize(&cont);

	return 0;
}
```

#####Pin/Port Configuration

LIBHD4480 requires at least one port to operate in 4-bit mode (two ports in 8-bit mode). Start by defining the control (select/direction/enable) pin 
configurations. In the example we chose PORTD pins 2 through 4. Next, select the data port.

```c
#define PIN_CTRL_E 2 // PD2
#define PIN_CTRL_RS 4 // PD4
#define PIN_CTRL_RW 3 // PD3
#define PORT_DATA B // PORTB
#define PORT_CTRL D // PORTD
```

**NOTE:** It is possible to combine control and data ports when using 4-bit mode. However, keep in mind that data in 4-bit mode occupies the lower 
nibble of the port. In this case, make sure that the control pins use pins 4 - 7!

#####Library Initialization

LIBHD4480 is initialized as the first step. No device manipulations should occur prior to this call to avoid any undesired/undefined behavior.

```c
	hd44780_initialize(&cont, DIMENSION_16_2, INTERFACE_4_BIT, FONT_EN_JP, 
			PORT_DATA, PORT_CTRL, PIN_CTRL_RS, PIN_CTRL_RW, PIN_CTRL_E);
```

As part of the initialization, various configuration flags are passed into this routine. All the possible inputs can be found in the 
the header file ```hd44780.h```, under ```./src/lib/include```:

```c
/**
 * Dimension type
 */
enum {
	DIMENSION_16_1 = 0,			// 16x1 (0-0xf)
	DIMENSION_16_2,				// 16x2 (0-0xf, 0x40-0x4f)
	DIMENSION_16_4,				// 16x4 (0-0xf, 0x40-0x4f, 0x10-0x1f, 0x50-0x5f)
	DIMENSION_20_2,				// 20x2 (0-0x13, 0x40-0x53)
	DIMENSION_20_4,				// 20x4 (0-0x13, 0x40-0x53, 0x14-0x27, 0x54-0x67)
	DIMENSION_40_2,				// 40x2 (0-0x27, 0x40-0x67)
};

/**
 * Font table type
 */
enum {
	FONT_EN_JP = 0,				// English/Japanese character set
	FONT_EUROPE_1,				// Western Europe 1 character set
	FONT_EN_RU,				// English/Russian character set
	FONT_EUROPE_2,				// Western Europe 2 character set
};

/**
 * Interface type
 */
enum {
	INTERFACE_4_BIT = 0,			// all commands sent in 4-bit segments
	INTERFACE_8_BIT,			// all comamnds sent in 8-bit segments
};
```

#####Printing UART Input

After initialization, we are ready to manipulate the device. As a first step, we configure the cursor to show with blink enabled:

```c
hd44780_cursor(&cont, CURSOR_ON, CURSOR_BLINK_ON);
```

Next we wait for input through the UART channel we opened:

```c
while(1) {
	hd44780_display_putc(&cont, uart_read());
}
```

**NOTE:** To extend this example, it might be useful to intercept control keys, such as CTRL^C, etc.

#####Library Uninitialization

Once we are done, LIBHD4480 is uninitialized as the final step. No further device manipulations should occur.

```c
hd44780_uninitialize(&cont);
```

####Adding Custom Panel Dimensions

In-order to handle the addressing scheme used in HD44780 panels, every new panel dimension will require a set of row offsets. However, it is fairly 
trivial to add these new dimensions to LIBHD4480. Most of the time, the dimension offsets can be found in the panels specification document.

#####Start by modifying the headers dimension enum:

```c
/**
 * Dimension type
 */
enum {
	DIMENSION_16_1 = 0,			// 16x1 (0-0xf)
	DIMENSION_16_2,				// 16x2 (0-0xf, 0x40-0x4f)
	DIMENSION_16_4,				// 16x4 (0-0xf, 0x40-0x4f, 0x10-0x1f, 0x50-0x5f)
	DIMENSION_20_2,				// 20x2 (0-0x13, 0x40-0x53)
	DIMENSION_20_4,				// 20x4 (0-0x13, 0x40-0x53, 0x14-0x27, 0x54-0x67)
	DIMENSION_40_2,				// 40x2 (0-0x27, 0x40-0x67)

	// add new dimensions here
};

#define DIMENSION_TYPE_MAX DIMENSION_40_2 // make sure to advance this if the new dimension is at the end of the enum
```

This enum value will be used during initialization to configure the device state.

#####Next, modify LIBHD4480's source file:

```c
static const uint8_t DIMESION_COLUMN_LEN[] = {
	16, 16, 16, 20, 20, 40,

	// add new column dimensions here
	};

static const uint8_t DIMESION_ROW_LEN[] = {
	1, 2, 4, 2, 4, 2,

	// add new row dimensions here
	};

static const uint8_t *DIMESION_ROW_OFF[] = {
	(uint8_t *) "\0",		// 16x1
	(uint8_t *) "\0\x40",		// 16x2
	(uint8_t *) "\0\x40\x10\x50",	// 16x4
	(uint8_t *) "\0\x40",		// 20x2
	(uint8_t *) "\0\x40\x14\x54",	// 20x4
	(uint8_t *) "\0\x40",		// 40x2

	// add new dimension offsets here
	};
```

As an example, we will add a hypothetical 8x2 panel, whose addressing offset is 0 for the first row, and 0x40 for the second:

#####Header file changes

```c
/**
 * Dimension type
 */
enum {
	DIMENSION_16_1 = 0,			// 16x1 (0-0xf)
	DIMENSION_16_2,				// 16x2 (0-0xf, 0x40-0x4f)
	DIMENSION_16_4,				// 16x4 (0-0xf, 0x40-0x4f, 0x10-0x1f, 0x50-0x5f)
	DIMENSION_20_2,				// 20x2 (0-0x13, 0x40-0x53)
	DIMENSION_20_4,				// 20x4 (0-0x13, 0x40-0x53, 0x14-0x27, 0x54-0x67)
	DIMENSION_40_2,				// 40x2 (0-0x27, 0x40-0x67)

	DIMENSION_8_2_EXAMPLE,			// new dimension
};

#define DIMENSION_TYPE_MAX DIMENSION_8_2_EXAMPLE // advanced to the new last enum value
```

#####Source file changes

```c
static const uint8_t DIMESION_COLUMN_LEN[] = {
	16, 16, 16, 20, 20, 40,

	8, // new dimension column count
	};

static const uint8_t DIMESION_ROW_LEN[] = {
	1, 2, 4, 2, 4, 2,

	2, // new dimension column count
	};

static const uint8_t *DIMESION_ROW_OFF[] = {
	(uint8_t *) "\0",		// 16x1
	(uint8_t *) "\0\x40",		// 16x2
	(uint8_t *) "\0\x40\x10\x50",	// 16x4
	(uint8_t *) "\0\x40",		// 20x2
	(uint8_t *) "\0\x40\x14\x54",	// 20x4
	(uint8_t *) "\0\x40",		// 40x2

	(uint8_t *) "\0\x40", // new dimension offset
	};
```

#####Finally, during initialization, we use the newly created dimension:

```c
	hd44780_initialize(&cont, DIMENSION_8_2_EXAMPLE, INTERFACE_4_BIT, FONT_EN_JP, 
			PORT_DATA, PORT_CTRL, PIN_CTRL_RS, PIN_CTRL_RW, PIN_CTRL_E);
```

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
