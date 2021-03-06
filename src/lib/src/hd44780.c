/**
 * libhd44780
 * Copyright (C) 2015 David Jolly
 * ----------------------
 *
 * libhd44780 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libhd44780 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <util/delay.h>
#include "../include/hd44780.h"

#define COMMAND_ADDRESS_SET 0x80
#define COMMAND_CURSOR_HOME 0x2
#define COMMAND_DISPLAY_CLEAR 0x1
#define COMMAND_DISPLAY_SET 0x8
#define COMMAND_ENTRY_MODE 0x4
#define COMMAND_FUNCTION_SET 0x28

#define DDR_OUTPUT_4 0xf
#define DDR_OUTPUT_8 0xff

#define DELAY_COMMAND 50 // us
#define DELAY_INITIALIZE 50 // ms
#define DELAY_LATCH 10 // us

#define FLAG_BUSY_4 0x8
#define FLAG_BUSY_8 0x80
#define FLAG_CURSOR_BLINK 0x1
#define FLAG_CURSOR_SHOW 0x2
#define FLAG_DIRECTION_INPUT 1
#define FLAG_DIRECTION_OUTPUT 0
#define FLAG_DISPLAY_SHOW 0x4
#define FLAG_INITIALIZED 0xFF38
#define FLAG_INTERFACE 0x10
#define FLAG_SHIFT_RIGHT 0x2

#define SELECT_COMMAND 0
#define SELECT_DATA 1

static const uint8_t DIMESION_COLUMN_LEN[] = {
	16, 16, 16, 20, 20, 40,
	};

#define DIMENSION_COLUMN_LENGTH(_TYPE_) \
	((_TYPE_) > DIMENSION_TYPE_MAX ? 0 : DIMESION_COLUMN_LEN[_TYPE_])

static const uint8_t DIMESION_ROW_LEN[] = {
	1, 2, 4, 2, 4, 2,
	};

#define DIMENSION_ROW_LENGTH(_TYPE_) \
	((_TYPE_) > DIMENSION_TYPE_MAX ? 0 : DIMESION_ROW_LEN[_TYPE_])

static const uint8_t *DIMESION_ROW_OFF[] = {
	(uint8_t *) "\0",		// 16x1
	(uint8_t *) "\0\x40",		// 16x2
	(uint8_t *) "\0\x40\x10\x50",	// 16x4
	(uint8_t *) "\0\x40",		// 20x2
	(uint8_t *) "\0\x40\x14\x54",	// 20x4
	(uint8_t *) "\0\x40",		// 40x2
	};

#define DIMENSION_ROW_OFFSET(_TYPE_, _ROW_) \
	((_TYPE_) > DIMENSION_TYPE_MAX ? 0 : \
	((_ROW_) >= DIMENSION_ROW_LENGTH(_ROW_) ? 0 : \
	DIMESION_ROW_OFF[_TYPE_][_ROW_]))

inline void 
busy_wait_4(
	__in hdcont_t *context
	)
{
	uint8_t busy;

	if(context) {
		*context->comm.ddr_data &= ~DDR_OUTPUT_4;
		*context->comm.port_data &= ~DDR_OUTPUT_4;
		*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		*context->comm.port_control |= _BV(context->comm.pin_control_direction);

		do {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
			*context->comm.port_control |= _BV(context->comm.pin_control_enable);
			_delay_us(DELAY_LATCH);
			busy = (*context->comm.port_data & FLAG_BUSY_4);
			*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
			*context->comm.port_control |= _BV(context->comm.pin_control_enable);
			_delay_us(DELAY_COMMAND);
			*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
		} while(busy);
	}
}

inline void 
busy_wait_8(
	__in hdcont_t *context
	)
{
	uint8_t busy;

	if(context) {
		*context->comm.ddr_data &= ~DDR_OUTPUT_8;
		*context->comm.port_data = 0;
		*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		*context->comm.port_control |= _BV(context->comm.pin_control_direction);

		do {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
			*context->comm.port_control |= _BV(context->comm.pin_control_enable);
			_delay_us(DELAY_LATCH);
			busy = (*context->comm.port_data & FLAG_BUSY_8);
			*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
		} while(busy);
	}
}

void 
hd44780_command_4_nibble(
	__in hdcont_t *context,
	__in uint8_t data
	)
{
	if(context) {
		*context->comm.ddr_data |= DDR_OUTPUT_4;
		*context->comm.port_data &= ~DDR_OUTPUT_4;
		*context->comm.port_data |= data;
		*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
		*context->comm.port_control |= _BV(context->comm.pin_control_enable);
		_delay_us(DELAY_COMMAND);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
	}
}

void 
hd44780_command_4(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	)
{
	if(context) {

		if(select) {
			*context->comm.port_control |= _BV(context->comm.pin_control_select);
		} else {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		}

		if(direction) {
			*context->comm.port_control |= _BV(context->comm.pin_control_direction);
		} else {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_direction);
		}

		hd44780_command_4_nibble(context, data >> 4);
		hd44780_command_4_nibble(context, data);
		busy_wait_4(context);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_direction);
		*context->comm.port_data &= ~DDR_OUTPUT_4;
	}
}

void 
hd44780_command_8(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	)
{
	if(context) {

		if(select) {
			*context->comm.port_control |= _BV(context->comm.pin_control_select);
		} else {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		}

		if(direction) {
			*context->comm.port_control |= _BV(context->comm.pin_control_direction);
		} else {
			*context->comm.port_control &= ~_BV(context->comm.pin_control_direction);
		}

		*context->comm.ddr_data = DDR_OUTPUT_8;
		*context->comm.port_data = data;
		*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
		*context->comm.port_control |= _BV(context->comm.pin_control_enable);
		_delay_us(DELAY_COMMAND);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_enable);
		busy_wait_8(context);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_select);
		*context->comm.port_control &= ~_BV(context->comm.pin_control_direction);
		*context->comm.port_data = 0;
	}
}

void 
hd44780_command(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	)
{
	if(context) {

		if(context->interface) {
			hd44780_command_8(context, select, direction, data);
		} else {
			hd44780_command_4(context, select, direction, data);
		}
	}
}

void 
hd44780_cursor(
	__in hdcont_t *context,
	__in uint8_t show,
	__in uint8_t blink
	)
{
	uint8_t data = COMMAND_DISPLAY_SET;

	if(context) {

		if(blink) {
			data |= FLAG_CURSOR_BLINK;
		}

		if(show) {
			data |= FLAG_CURSOR_SHOW;
		}

		if(context->state.display_show) {
			data |= FLAG_DISPLAY_SHOW;
		}

		hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, data);
		context->state.cursor_blink = blink;
		context->state.cursor_show = show;
	}
}

void 
hd44780_cursor_home(
	__in hdcont_t *context
	)
{
	if(context) {
		hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, COMMAND_CURSOR_HOME);
		context->state.current_column = 0;
		context->state.current_row = 0;
	}
}

void 
hd4480_cursor_set(
	__in hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row
	)
{
	uint8_t address;

	if(context) {
		address = DIMENSION_ROW_OFFSET(context->dimension, row) + column;
		hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
				COMMAND_ADDRESS_SET | address);
		context->state.current_column = column;
		context->state.current_row = row;
	}
}

void 
hd44780_display(
	__in hdcont_t *context,
	__in uint8_t show
	)
{
	uint8_t data = COMMAND_DISPLAY_SET;

	if(context) {

		if(show) {
			data |= FLAG_DISPLAY_SHOW;
		}

		if(context->state.cursor_blink) {
			data |= FLAG_CURSOR_BLINK;
		}

		if(context->state.cursor_show) {
			data |= FLAG_CURSOR_SHOW;
		}

		hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, data);
		context->state.display_show = show;
	}
}

void 
hd44780_display_clear(
	__in hdcont_t *context
	)
{
	if(context) {
		hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, COMMAND_DISPLAY_CLEAR);
	}
}

void 
hd44780_display_putc(
	__in hdcont_t *context,
	__in char input
	)
{
	if(context) {

		if((context->state.current_column >= context->state.dimension_column)
				&& (context->state.current_row >= (context->state.dimension_row - 1))) {
			hd44780_display_clear(context);
			hd44780_cursor_home(context);
		} else if(context->state.current_column >= context->state.dimension_column) {
			context->state.current_column = 0;			
			hd4480_cursor_set(context, context->state.current_column, 
					++context->state.current_row);
		}

		hd44780_command(context, SELECT_DATA, FLAG_DIRECTION_OUTPUT, input);
		++context->state.current_column;
	}
}

void 
hd44780_display_puts(
	__in hdcont_t *context,
	__in char *input
	)
{
	if(context && input) {

		while(*input != '\0') {
			hd44780_display_putc(context, *input++);
		}
	}
}

void 
_hd44780_initialize(
	__out hdcont_t *context,
	__in uint8_t dimension,
	__in uint8_t interface,
	__in uint8_t font,
	__in volatile uint8_t *ddr_data,
	__in volatile uint8_t *port_data,
	__in volatile uint8_t *ddr_control,
	__in volatile uint8_t *port_control,
	__in uint8_t pin_control_select,
	__in uint8_t pin_control_direction,
	__in uint8_t pin_control_enable
	)
{
	if(context && ddr_control && ddr_data && port_control && port_data) {
		context->dimension = dimension;
		context->interface = interface;
		context->state.current_column = 0;
		context->state.current_row = 0;
		context->state.cursor_blink = CURSOR_BLINK_OFF;
		context->state.cursor_show = CURSOR_OFF;
		context->state.dimension_column = DIMENSION_COLUMN_LENGTH(dimension);
		context->state.dimension_row = DIMENSION_ROW_LENGTH(dimension);
		context->state.display_show = DISPLAY_OFF;
		context->comm.ddr_control = ddr_control;
		context->comm.ddr_data = ddr_data;
		context->comm.port_control = port_control;
		context->comm.port_data = port_data;
		context->comm.pin_control_direction = pin_control_direction;
		context->comm.pin_control_enable = pin_control_enable;
		context->comm.pin_control_select = pin_control_select;	
		*context->comm.ddr_control |= (_BV(context->comm.pin_control_direction) 
				| _BV(context->comm.pin_control_enable) 
				| _BV(context->comm.pin_control_select)); 
		*context->comm.port_control &= ~(_BV(context->comm.pin_control_direction) 
				| _BV(context->comm.pin_control_enable) 
				| _BV(context->comm.pin_control_select));
		_delay_ms(DELAY_INITIALIZE);

		if(context->interface) {
			*context->comm.ddr_data = DDR_OUTPUT_8;
			*context->comm.port_data = 0;
			hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
					COMMAND_FUNCTION_SET | FLAG_INTERFACE | font);
			hd44780_cursor(context, CURSOR_OFF, CURSOR_BLINK_OFF);
			hd44780_display(context, DISPLAY_OFF);
			hd44780_display_clear(context);
			hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
					COMMAND_ENTRY_MODE | FLAG_SHIFT_RIGHT);
			hd44780_cursor_home(context);
			hd44780_display(context, DISPLAY_ON);
			hd44780_cursor(context, CURSOR_ON, CURSOR_BLINK_ON);
		} else {
			*context->comm.ddr_data |= DDR_OUTPUT_4;
			*context->comm.port_data &= ~DDR_OUTPUT_4;
			hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
					COMMAND_FUNCTION_SET | FLAG_INTERFACE | font);
			hd44780_command_4_nibble(context, COMMAND_CURSOR_HOME);
			hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
					COMMAND_FUNCTION_SET | font);
			hd44780_cursor(context, CURSOR_OFF, CURSOR_BLINK_OFF);
			hd44780_display(context, DISPLAY_OFF);
			hd44780_display_clear(context);
			hd44780_command(context, SELECT_COMMAND, FLAG_DIRECTION_OUTPUT, 
					COMMAND_ENTRY_MODE | FLAG_SHIFT_RIGHT);
			hd44780_cursor_home(context);
			hd44780_display(context, DISPLAY_ON);
			hd44780_cursor(context, CURSOR_ON, CURSOR_BLINK_ON);
		}
	}
}

void 
hd44780_uninitialize(
	__out hdcont_t *context
	)
{
	if(context) {
		hd44780_display_clear(context);
		hd44780_cursor_home(context);
		hd44780_cursor(context, CURSOR_OFF, CURSOR_BLINK_OFF);
		hd44780_display(context, DISPLAY_OFF);
		*context->comm.port_control &= ~(_BV(context->comm.pin_control_direction) 
				| _BV(context->comm.pin_control_enable) 
				| _BV(context->comm.pin_control_select));
		*context->comm.ddr_control &= ~(_BV(context->comm.pin_control_direction) 
				| _BV(context->comm.pin_control_enable) 
				| _BV(context->comm.pin_control_select));

		if(context->interface) {
			*context->comm.port_data = 0;
			*context->comm.ddr_data &= ~DDR_OUTPUT_8;
		} else {
			*context->comm.port_data &= ~DDR_OUTPUT_4;
			*context->comm.ddr_data &= ~DDR_OUTPUT_4;
		}

		context->state.current_column = 0;
		context->state.current_row = 0;
		context->state.cursor_blink = CURSOR_BLINK_OFF;
		context->state.cursor_show = CURSOR_OFF;
		context->state.dimension_column = 0;
		context->state.dimension_row = 0;
		context->state.display_show = DISPLAY_OFF;
		context->interface = 0;
		context->dimension = 0;
	}
}
