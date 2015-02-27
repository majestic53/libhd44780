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

#ifndef HD44780_H_
#define HD44780_H_

#include <stdint.h>
#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __in
#define __in
#endif // __in
#ifndef __out
#define __out
#endif // __out

typedef struct _hdcont_comm_t {
	volatile uint8_t *ddr_control;
	volatile uint8_t *ddr_data;
	uint8_t pin_control_direction;
	uint8_t pin_control_enable;
	uint8_t pin_control_select;
	volatile uint8_t *port_control;
	volatile uint8_t *port_data;
} hdcont_comm_t;

typedef struct _hdcont_state_t {
	uint8_t current_column;
	uint8_t current_row;
	uint8_t cursor_blink;
	uint8_t cursor_show;
	uint8_t dimension_column;
	uint8_t dimension_row;
	uint8_t display_show;
} hdcont_state_t;

typedef struct _hdcont_t {
	uint8_t interface;
	hdcont_comm_t comm;
	hdcont_state_t state;
} hdcont_t;

/**
 * Setup routines
 */

#define FONT_EN_JP 0
#define FONT_EUROPE_1 1
#define FONT_EN_RU 2
#define FONT_EUROPE_2 3

#define INTERFACE_4_BIT 0
#define INTERFACE_8_BIT 1

#define SHIFT_LEFT 0
#define SHIFT_RIGHT 1

#define DEFINE_DDR(_BNK_) DDR ## _BNK_
#define DEFINE_PIN(_BNK_, _PIN_) P ## _BNK_ ## _PIN_
#define DEFINE_PORT(_BNK_) PORT ## _BNK_

#define hd44780_initialize(_CONT_, _COL_, _ROW_, _INTER_, _FONT_, _SHFT_, _DATA_, \
		_CTRL_, _RS_, _RW_, _E_) \
	_hd44780_initialize(_CONT_, _COL_, _ROW_, _INTER_, _FONT_, _SHFT_, \
	&DEFINE_DDR(_DATA_), &DEFINE_PORT(_DATA_), &DEFINE_DDR(_CTRL_), \
	&DEFINE_PORT(_CTRL_), DEFINE_PIN(_CTRL_, _RS_), DEFINE_PIN(_CTRL_, _RW_), \
	DEFINE_PIN(_CTRL_, _E_))
void _hd44780_initialize(
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
	);

void hd44780_uninitialize(
	__out hdcont_t *context
	);

/**
 * Cursor routines
 */

#define CURSOR_BLINK_OFF 0
#define CURSOR_BLINK_ON 1
#define CURSOR_OFF 0
#define CURSOR_ON 1

void hd44780_cursor(
	__in hdcont_t *context,
	__in uint8_t show,
	__in uint8_t blink
	);

void hd44780_cursor_home(
	__in hdcont_t *context
	);

void hd4480_cursor_set(
	__in hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row
	);

/**
 * Display routines
 */

#define DISPLAY_OFF 0
#define DISPLAY_ON 1

void hd44780_display(
	__in hdcont_t *context,
	__in uint8_t show
	);

void hd44780_display_clear(
	__in hdcont_t *context
	);

void hd44780_display_putc(
	__in hdcont_t *context,
	__in char input
	);

void hd44780_display_puts(
	__in hdcont_t *context,
	__in char *input
	);

/**
 * Device routines
 */

void hd44780_command(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HD44780_H_
