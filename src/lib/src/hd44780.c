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

#include <string.h>
#include <util/delay.h>
#include "../include/hd44780.h"

#ifndef NDEBUG
#define BAUD 9600

#include <stdarg.h>
#include <stdio.h>
#include <util/setbaud.h>

#define TRACE_ERR_ENCODE "TRACE ENCODING FAILED!"
#define TRACE_ERR_TOO_LONG "TRACE IS TOO LONG!"
#define TRACE_BUF_LEN_MAX 0x100

static char trace_buf[TRACE_BUF_LEN_MAX] = {0};

inline void 
trace_character(
	__in char input
	)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = input;
}

inline void
trace_message(
	__in const char *message,
	__in const char *prefix,
	__in const char *function,
	__in const char *file,
	__in const char *line,
	__in bool verbose
	)
{
	uint16_t iter;

	if(verbose) {

		if(prefix) {
			iter = 0;
		
			while(prefix[iter]) {
				trace_character(prefix[iter++]);
			}
		}

		if(function) {
			iter = 0;

			while(function[iter]) {
				trace_character(function[iter++]);
			}

			trace_character(' ');
		}

		trace_character('(');

		if(file) {
			iter = 0;

			while(file[iter]) {
				trace_character(file[iter++]);
			}

			trace_character(':');
		}

		if(line) {
			iter = 0;

			while(line[iter]) {
				trace_character(line[iter++]);
			}
		}

		trace_character(')');

		if(message) {
			trace_character(':');
			trace_character(' ');
		}
	}

	if(message) {
		iter = 0;

		while(message[iter]) {
			trace_character(message[iter++]);
		}
	}

	trace_character('\r');
	trace_character('\n');
}

inline void
trace_initialize(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif // USE_2X
	UCSR0B = (_BV(TXEN0) | _BV(RXEN0));
	UCSR0C = (_BV(UCSZ01) | _BV(UCSZ00));
}

inline char *
trace_string(
	__in const char *format,
	...
	)
{
	int len;
	va_list arg_lst;
	char *result = trace_buf;

	memset(trace_buf, 0, TRACE_BUF_LEN_MAX);
	va_start(arg_lst, format);
	len = vsnprintf(trace_buf, TRACE_BUF_LEN_MAX, format, arg_lst);
	va_end(arg_lst);

	if(len < 0) {
		result = TRACE_ERR_ENCODE;
	} else if(len > TRACE_BUF_LEN_MAX) {
		result = TRACE_ERR_TOO_LONG;
	}

	return result;
}

#define TRACE_INITIALIZE() trace_initialize()
#define TRACE_MESSAGE(_FORMAT_, ...) \
	trace_message(trace_string(_FORMAT_, __VA_ARGS__), NULL, __func__, \
	__FILE__, CONCAT_STR(__LINE__), true)
#define TRACE_ENTRY() \
	trace_message(NULL, "+", __func__, __FILE__, CONCAT_STR(__LINE__), true)
#define TRACE_ENTRY_MESSAGE(_FORMAT_, ...) \
	trace_message(trace_string(_FORMAT_, __VA_ARGS__), "+", __func__, \
	__FILE__, CONCAT_STR(__LINE__), true)
#define TRACE_EXIT() \
	trace_message(NULL, "-", __func__, __FILE__, CONCAT_STR(__LINE__), true)
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...) \
	trace_message(trace_string(_FORMAT_, __VA_ARGS__), "-", __func__, \
	__FILE__, CONCAT_STR(__LINE__), true)
#else
#define TRACE_INITIALIZE()
#define TRACE_MESSAGE(_FORMAT_, ...)
#define TRACE_ENTRY()
#define TRACE_ENTRY_MESSAGE(_FORMAT_, ...)
#define TRACE_EXIT()
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...)
#endif // NDEBUG

#define COMMAND_ADDRESS_SET 0x80
#define COMMAND_CURSOR_HOME 0x2
#define COMMAND_DISPLAY_CLEAR 0x1
#define COMMAND_DISPLAY_SET 0x8
#define COMMAND_ENTRY_MODE 0x4
#define COMMAND_FUNCTION_SET 0x28
#define CURSOR_BLINK 0x1
#define CURSOR_SHOW 0x2
#define DDR_INPUT 0
#define DDR_OUTPUT UINT8_MAX
#define DELAY_COMMAND 50 // us
#define DELAY_INITIALIZE 50 // ms
#define DELAY_LATCH 10 // us
#define DIRECTION_INPUT 1
#define DIRECTION_OUTPUT 0
#define DISPLAY_SHOW 0x4
#define FLAG_BUSY 0x80
#define FLAG_INITIALIZED 0xFF38
#define FLAG_INTERFACE 0x10
#define FLAG_SHIFT_RIGHT 0x2
#define ROW_ADDRESS_SCALAR 0x40
#define SELECT_COMMAND 0
#define SELECT_DATA 1

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_WEEK 1509
#define VERSION_REVISION 4

#define _CONCAT_STR(_STR_) # _STR_
#define CONCAT_STR(_STR_) _CONCAT_STR(_STR_)

inline hderr_t 
sanitize(
	__in hdcont_t *context
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_ENTRY();

	if(!context || !context->ddr_control || !context->ddr_data 
			|| !context->port_control || !context->port_data) {
		result = HD_ERR_INVALID_ARGUMENT;
	} else if(context->flag != FLAG_INITIALIZED) {
		result = HD_ERR_UNINITIALIZED;
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

inline hderr_t 
busy_wait(
	__in hdcont_t *context
	)
{
	uint8_t busy;
	hderr_t result = sanitize(context);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {
		*context->ddr_data = DDR_INPUT;
		*context->port_control &= ~_BV(context->pin_control_select);
		*context->port_control |= _BV(context->pin_control_direction);

		do {
			*context->port_control &= ~_BV(context->pin_control_enable);
			*context->port_control |= _BV(context->pin_control_enable);
			_delay_us(DELAY_LATCH);
			busy = ((*context->port_data & FLAG_BUSY) == 1);
			*context->port_control &= ~_BV(context->pin_control_enable);
		} while(busy);
	}

	*context->port_control &= ~_BV(context->pin_control_select);
	*context->port_control &= ~_BV(context->pin_control_direction);
	*context->ddr_data = DDR_OUTPUT;

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_command(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	)
{
	hderr_t result = sanitize(context);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		if(select) {
			*context->port_control |= _BV(context->pin_control_select);
		} else {
			*context->port_control &= ~_BV(context->pin_control_select);
		}

		if(direction) {
			*context->port_control |= _BV(context->pin_control_direction);
		} else {
			*context->port_control &= ~_BV(context->pin_control_direction);
		}

		*context->ddr_data = DDR_OUTPUT;
		*context->port_data = data;
		*context->port_control &= ~_BV(context->pin_control_enable);
		*context->port_control |= _BV(context->pin_control_enable);
		_delay_us(DELAY_COMMAND);
		*context->port_control &= ~_BV(context->pin_control_enable);

		result = busy_wait(context);
		if(!HD_ERR_SUCCESS(result)) {
			goto exit;
		}

		*context->port_control &= ~_BV(context->pin_control_select);
		*context->port_control &= ~_BV(context->pin_control_direction);
		*context->port_data = 0;
	}

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor(
	__in hdcont_t *context,
	__in bool show,
	__in bool blink
	)
{
	hderr_t result = sanitize(context);
	uint8_t data = COMMAND_DISPLAY_SET;

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		if(blink) {
			data |= CURSOR_BLINK;
		}

		if(show) {
			data |= CURSOR_SHOW;
		}

		if(context->display_show) {
			data |= DISPLAY_SHOW;
		}

		result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, data);
		if(HD_ERR_SUCCESS(result)) {
			context->cursor_blink = blink;
			context->cursor_show = show;
		}
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_home(
	__in hdcont_t *context
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_ENTRY();

	result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, 
			COMMAND_CURSOR_HOME);

	if(HD_ERR_SUCCESS(result)) {
		context->current_column = 0;
		context->current_row = 0;
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd4480_cursor_set(
	__in hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row
	)
{
	uint8_t address;
	hderr_t result = sanitize(context);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		if((column >= context->dimension_column)
				|| (row >= context->dimension_row)) {
			result = HD_ERR_INVALID_ARGUMENT;
			goto exit;
		}

		address = (row * ROW_ADDRESS_SCALAR) + column;
		result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, 
				COMMAND_ADDRESS_SET | address);

		if(HD_ERR_SUCCESS(result)) {
			context->current_column = column;
			context->current_row = row;
		}
	}

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_display(
	__in hdcont_t *context,
	__in bool show
	)
{
	hderr_t result = sanitize(context);
	uint8_t data = COMMAND_DISPLAY_SET;

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		if(show) {
			data |= DISPLAY_SHOW;
		}

		if(context->cursor_blink) {
			data |= CURSOR_BLINK;
		}

		if(context->cursor_show) {
			data |= CURSOR_SHOW;
		}

		result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, data);
		if(HD_ERR_SUCCESS(result)) {
			context->display_show = show;
		}
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_display_clear(
	__in hdcont_t *context
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_ENTRY();

	result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, 
			COMMAND_DISPLAY_CLEAR);

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_display_put(
	__in hdcont_t *context,
	__in char input
	)
{
	hderr_t result = sanitize(context);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		if((context->current_column >= context->dimension_column)
				&& (context->current_row >= (context->dimension_row - 1))) {

			result = hd44780_display_clear(context);
			if(!HD_ERR_SUCCESS(result)) {
				goto exit;
			}

			result = hd44780_cursor_home(context);
			if(!HD_ERR_SUCCESS(result)) {
				goto exit;
			}
		} else if(context->current_column >= context->dimension_column) {
			context->current_column = 0;			

			result = hd4480_cursor_set(context, context->current_column, 
					++context->current_row);
			if(!HD_ERR_SUCCESS(result)) {
				goto exit;
			}
		}

		result = hd44780_command(context, SELECT_DATA, DIRECTION_OUTPUT, input);
		++context->current_column;
	}

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
_hd44780_initialize(
	__out hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row,
	__in uint8_t interface,
	__in uint8_t font,
	__in uint8_t shift,
	__in volatile uint8_t *ddr_data,
	__in volatile uint8_t *port_data,
	__in volatile uint8_t *ddr_control,
	__in volatile uint8_t *port_control,
	__in uint8_t pin_control_select,
	__in uint8_t pin_control_direction,
	__in uint8_t pin_control_enable
	)
{
	uint8_t data;
	hderr_t result = HD_ERR_NONE;

	TRACE_INITIALIZE();
	TRACE_ENTRY();

	if(!context || !column || !ddr_control || !ddr_data 
			|| !port_control || !port_data || !row
			|| (font > FONT_MAX)
			|| (interface > INTERFACE_MAX)
			|| (shift > SHIFT_MAX)) {
		result = HD_ERR_INVALID_ARGUMENT;
		goto exit;
	}

	context->current_column = 0;
	context->current_row = 0;
	context->cursor_blink = false;
	context->cursor_show = false;
	context->ddr_control = ddr_control;
	context->ddr_data = ddr_data;
	context->dimension_column = column;
	context->dimension_row = row;
	context->display_show = false;
	context->port_control = port_control;
	context->port_data = port_data;
	context->pin_control_direction = pin_control_direction;
	context->pin_control_enable = pin_control_enable;
	context->pin_control_select = pin_control_select;	
	context->flag = FLAG_INITIALIZED;
	*context->ddr_control |= (_BV(context->pin_control_direction) | _BV(context->pin_control_enable) 
			| _BV(context->pin_control_select)); 
	*context->port_control &= ~(_BV(context->pin_control_direction) | _BV(context->pin_control_enable) 
			| _BV(context->pin_control_select));
	_delay_ms(DELAY_INITIALIZE);
	*context->ddr_data = DDR_OUTPUT;
	*context->port_data = 0;

	data = COMMAND_FUNCTION_SET | font;

	if(interface) {
		data |= FLAG_INTERFACE;
	}

	result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, data);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_cursor(context, false, false);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_display(context, false);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_display_clear(context);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	data = COMMAND_ENTRY_MODE;

	if(shift) {
		data |= FLAG_SHIFT_RIGHT;
	}

	result = hd44780_command(context, SELECT_COMMAND, DIRECTION_OUTPUT, data);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_cursor_home(context);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_display(context, true);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	result = hd44780_cursor(context, true, true);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

void 
hd44780_uninitialize(
	__out hdcont_t *context
	)
{
	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(sanitize(context))) {

		if(!HD_ERR_SUCCESS(hd44780_display_clear(context))) {
			goto exit;
		}

		if(!HD_ERR_SUCCESS(hd44780_cursor_home(context))) {
			goto exit;
		}

		if(!HD_ERR_SUCCESS(hd44780_cursor(context, false, false))) {
			goto exit;
		}

		if(!HD_ERR_SUCCESS(hd44780_display(context, false))) {
			goto exit;
		}

		*context->port_control = 0;
		*context->port_data = 0;
		*context->ddr_control &= ~(_BV(context->pin_control_direction) | _BV(context->pin_control_enable) 
				| _BV(context->pin_control_select));
		*context->ddr_data = DDR_INPUT;
		memset(context, 0, sizeof(hdcont_t));
	}

exit:
	TRACE_EXIT();
}

const char *
hd44780_version(void)
{
	return CONCAT_STR(VERSION_MAJOR) "." CONCAT_STR(VERSION_MINOR) "." \
			CONCAT_STR(VERSION_WEEK) "." CONCAT_STR(VERSION_REVISION);
}
