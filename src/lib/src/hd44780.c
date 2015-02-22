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

#include <avr/io.h>
#include <util/delay.h>
#include "../include/hd44780.h"

#define BIT_CLEAR(_RES_, _VAL_) ((_RES_) &= ~(_VAL_))
#define BIT_SET(_RES_, _VAL_) ((_RES_) |= (_VAL_))

#ifndef NDEBUG
#define BAUD 9600
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
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
	trace_event(trace_str(_FORMAT_, __VA_ARGS__), NULL, __func__, __FILE__, CAT_STR(__LINE__), true)
#define TRACE_ENTRY() \
	trace_event(NULL, "+", __func__, __FILE__, CAT_STR(__LINE__), true)
#define TRACE_EXIT() \
	trace_event(NULL, "-", __func__, __FILE__, CAT_STR(__LINE__), true)
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...) \
	trace_event(trace_str(_FORMAT_, __VA_ARGS__), "-", __func__, __FILE__, CAT_STR(__LINE__), true)
#else
#define TRACE_INIT()
#define TRACE_EVENT(_FORMAT_, ...)
#define TRACE_ENTRY()
#define TRACE_EXIT()
#define TRACE_EXIT_MESSAGE(_FORMAT_, ...)
#endif // NDEBUG

#define API_INIT 0xFF39
#define API_VERSION 1

inline hderr_t 
check_input(
	__in hdcont_t *cont
	)
{
	hderr_t result = HD_ERR_NONE;

	if(!cont) {
		result = HD_ERR_INVALID;
	} else if(cont->init != API_INIT) {
		result = HD_ERR_UNINIT;
	}

	return result;
}

hderr_t 
hd44780_cursor_blink(
	__out hdcont_t *cont,
	__in bool set
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_home(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_left(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_right(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_set(
	__out hdcont_t *cont,
	__in uint8_t row,
	__in uint8_t col
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_cursor_show(
	__out hdcont_t *cont,
	__in bool set
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_init(
	__out hdcont_t *cont,
	__in uint8_t port_data,
	__in uint8_t port_ctrl,
	__in uint8_t pin_enab,
	__in uint8_t pin_sel,
	__in uint8_t pin_dir
	)
{
	hderr_t result = HD_ERR_NONE;

	TRACE_INIT();
	TRACE_ENTRY();

	if(cont->init == API_INIT) {
		result = HD_ERR_INVALID;
		goto exit;
	}

	// TODO

	cont->init = API_INIT;

exit:
	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_screen_clear(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_screen_left(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_screen_right(
	__out hdcont_t *cont
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

void 
hd44780_uninit(
	__in hdcont_t *cont
	)
{
	TRACE_ENTRY();

	if(check_input(cont)) {
		cont->ctrl.pin_dir = 0;
		cont->ctrl.pin_enab = 0;
		cont->ctrl.pin_sel = 0;
		cont->ctrl.port = 0;
		cont->data.port = 0;
		cont->init = 0;
	}

	TRACE_EXIT();
}

uint16_t 
hd44780_version(void)
{
	TRACE_ENTRY();
	TRACE_EXIT_MESSAGE("API=%i", API_VERSION);
	return API_VERSION;
}

hderr_t 
hd44780_write_char(
	__out hdcont_t *cont,
	__in char ch
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}

hderr_t 
hd44780_write_str(
	__out hdcont_t *cont,
	__in const char *str,
	__in uint16_t len
	)
{
	hderr_t result = check_input(cont);

	TRACE_ENTRY();

	if(HD_ERR_SUCCESS(result)) {

		// TODO
	}

	TRACE_EXIT_MESSAGE("Return Value: 0x%x", result);
	return result;
}
