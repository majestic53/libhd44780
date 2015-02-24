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

#define DELAY_COMMAND 50 //us
#define DELAY_INIT 50 //ms
#define DELAY_LATCH 10 //us

#define FLAG_BUSY 7 //bit
#define FLAG_INIT 0xFF38

#define VER_MAJ 0
#define VER_MIN 1
#define VER_WEEK 1509
#define VER_REV 2

#define WORD_LEN 8 //bits

#define BIT_CLR(_REG_, _BIT_) ((_REG_) &= ~(_BIT_))
#define BIT_SET(_REG_, _BIT_) ((_REG_) |= (_BIT_))

#define _STR_CAT(_STR_) # _STR_
#define STR_CAT(_STR_) _STR_CAT(_STR_)

#ifndef NDEBUG
#define BAUD 9600 //kb/sec
#include <stdarg.h>
#include <stdio.h>
#include <util/setbaud.h>

#define TRACE_ERR_ENCODE "[TRACE ENCODING FAILED]"
#define TRACE_ERR_TOO_LONG "[TRACE IS TOO LONG]"
#define TRACE_BUF_LEN_MAX 0x100

static char trace_buf[TRACE_BUF_LEN_MAX] = {0};

inline void 
trace_char(
	__in char ch
	)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = ch;
}

inline void
trace_event(
	__in const char *str,
	__in const char *prefix,
	__in const char *funct,
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
				trace_char(prefix[iter++]);
			}
		}

		if(funct) {
			iter = 0;

			while(funct[iter]) {
				trace_char(funct[iter++]);
			}

			trace_char(' ');
		}

		trace_char('(');

		if(file) {
			iter = 0;

			while(file[iter]) {
				trace_char(file[iter++]);
			}

			trace_char(':');
		}

		if(line) {
			iter = 0;

			while(line[iter]) {
				trace_char(line[iter++]);
			}
		}

		trace_char(')');

		if(str) {
			trace_char(':');
			trace_char(' ');
		}
	}

	if(str) {
		iter = 0;

		while(str[iter]) {
			trace_char(str[iter++]);
		}
	}

	trace_char('\r');
	trace_char('\n');
}

inline void
trace_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	BIT_SET(UCSR0A, _BV(U2X0));
#else
	BIT_CLEAR(UCSR0A, _BV(U2X0));
#endif // USE_2X
	UCSR0B = (_BV(TXEN0) | _BV(RXEN0));
	UCSR0C = (_BV(UCSZ01) | _BV(UCSZ00));
}

inline char *
trace_str(
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

#define _CAT_STR(_STR_) # _STR_
#define CAT_STR(_STR_) _CAT_STR(_STR_)
#define TRACE_INIT() trace_init()
#define TRACE_EVENT(_FORMAT_, ...) \
	trace_event(trace_str(_FORMAT_, __VA_ARGS__), NULL, __func__, \
	__FILE__, CAT_STR(__LINE__), true)
#define TRACE_ENTRY() \
	trace_event(NULL, "+", __func__, __FILE__, CAT_STR(__LINE__), true)
#define TRACE_ENTRY_MESSAGE(_FORMAT_, ...) \
	trace_event(trace_str(_FORMAT_, __VA_ARGS__), "+", __func__, \
	__FILE__, CAT_STR(__LINE__), true)
#define TRACE_EXIT() \
	trace_event(NULL, "-", __func__, __FILE__, CAT_STR(__LINE__), true)
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...) \
	trace_event(trace_str(_FORMAT_, __VA_ARGS__), "-", __func__, \
	__FILE__, CAT_STR(__LINE__), true)
#else
#define TRACE_INIT()
#define TRACE_EVENT(_FORMAT_, ...)
#define TRACE_ENTRY()
#define TRACE_ENTRY_MESSAGE(_FORMAT_, ...)
#define TRACE_EXIT()
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...)
#endif // NDEBUG

inline hderr_t 
sanitize(
	__in hdcont_t *cont
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_ENTRY();

	if(!cont || !cont->ddr_ctrl || !cont->ddr_data 
			|| !cont->port_ctrl || !cont->port_data) {
		result = HD_ERR_INVALID;
	} else if(cont->init != FLAG_INIT) {
		result = HD_ERR_UNINIT;
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

inline hderr_t 
busy_wait(
	__in hdcont_t *cont
	)
{
	uint8_t busy;
	hderr_t result = sanitize(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// set data pins (d0-d7) as input
		*cont->ddr_data = 0;

		// set rs/rw pins (low/high)
		BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rs));
		BIT_SET(*cont->port_ctrl, _BV(cont->pin_ctrl_rw));

		do {

			// latch command
			BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_e));
			BIT_SET(*cont->port_ctrl, _BV(cont->pin_ctrl_e));
			_delay_us(DELAY_LATCH);

			// check busy flag
			busy = (*cont->port_data & _BV(FLAG_BUSY));
			BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_e));
		} while(busy);
	}

	// clear rs/rw pins
	BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rs));
	BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rw));

	// set data pins (d0-d7) as output
	*cont->ddr_data = UINT8_MAX;

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_command(
	__in hdcont_t *cont,
	__in uint8_t rs,
	__in uint8_t rw,
	__in uint8_t data
	)
{
	hderr_t result = sanitize(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// set rs pin
		if(rs) {
			BIT_SET(*cont->port_ctrl, _BV(cont->pin_ctrl_rs));
		} else {
			BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rs));
		}
		
		// set rw pin
		if(rw) {
			BIT_SET(*cont->port_ctrl, _BV(cont->pin_ctrl_rw));
		} else {
			BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rw));
		}

		// set data pins (d0-d7) as output
		*cont->ddr_data = UINT8_MAX;

		// set data pins (d0-d7)
		*cont->port_data = data;

		// latch command
		BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_e));
		BIT_SET(*cont->port_ctrl, _BV(cont->pin_ctrl_e));
		_delay_us(DELAY_COMMAND);
		BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_e));

		// wait for the device to clear the busy flag
		result = busy_wait(cont);
		if(!HD_ERR_SUCCESS(result)) {
			goto exit;
		}

		// clear rs/rw/data (d0-d7) pins
		BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rs));
		BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_rw));
		*cont->port_data = 0;
	}

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
_hd44780_init(
	__out hdcont_t *cont,
	__in volatile uint8_t *ddr_data,
	__in volatile uint8_t *port_data,
	__in volatile uint8_t *ddr_ctrl,
	__in volatile uint8_t *port_ctrl,
	__in uint8_t pin_ctrl_rs,
	__in uint8_t pin_ctrl_rw,
	__in uint8_t pin_ctrl_e
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_INIT();
	TRACE_ENTRY();

	if(!cont || !ddr_ctrl || !ddr_data 
			|| !port_ctrl || !port_data) {
		result = HD_ERR_INVALID;
		goto exit;
	}

	cont->ddr_ctrl = ddr_ctrl;
	cont->ddr_data = ddr_data;
	cont->port_ctrl = port_ctrl;
	cont->port_data = port_data;
	cont->pin_ctrl_e = pin_ctrl_e;
	cont->pin_ctrl_rs = pin_ctrl_rs;
	cont->pin_ctrl_rw = pin_ctrl_rw;
	cont->init = FLAG_INIT;

	// set ctrl pins as output and zero
	BIT_SET(*cont->ddr_ctrl, _BV(cont->pin_ctrl_e) | _BV(cont->pin_ctrl_rs) 
			| _BV(cont->pin_ctrl_rw)); 
	BIT_CLR(*cont->port_ctrl, _BV(cont->pin_ctrl_e) | _BV(cont->pin_ctrl_rs) 
			| _BV(cont->pin_ctrl_rw));

	// wait for device initialization
	_delay_ms(DELAY_INIT);

	// set data pins as output and zero
	*cont->ddr_data = UINT8_MAX;
	*cont->port_data = 0;

	// TODO: remove after debugging
	result = hd44780_command(cont, 0, 0, 0xf);
	// ---

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

void 
hd44780_uninit(
	__out hdcont_t *cont
	)
{
	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(sanitize(cont))) {

		// TODO: clear screen, return home, turn off screen

		*cont->port_ctrl = 0;
		*cont->port_data = 0;
		*cont->ddr_ctrl = 0;
		*cont->ddr_data = 0;
		memset(cont, 0, sizeof(hdcont_t));
	}

	TRACE_EXIT();
}

const char *
hd44780_version(void)
{
	return STR_CAT(VER_MAJ) "." STR_CAT(VER_MIN) "." \
			STR_CAT(VER_WEEK) "." STR_CAT(VER_REV);
}
