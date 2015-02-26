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

#define NDEBUG

#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>

#ifndef __in
#define __in
#endif // __in
#ifndef __out
#define __out
#endif // __out

#define DEFINE_DDR(_BNK_) DDR ## _BNK_
#define DEFINE_PIN(_BNK_, _PIN_) P ## _BNK_ ## _PIN_
#define DEFINE_PORT(_BNK_) PORT ## _BNK_

enum {
	FONT_EN_JP = 0,
	FONT_EUROPE_1,
	FONT_EN_RU,
	FONT_EUROPE_2,
};

#define FONT_MAX FONT_EUROPE_2

enum {
	INTERFACE_4_BIT = 0,
	INTERFACE_8_BIT,
};

#define INTERFACE_MAX INTERFACE_8_BIT

enum {
	SHIFT_LEFT = 0,
	SHIFT_RIGHT,
};

#define SHIFT_MAX SHIFT_RIGHT

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum _hderr_t {
	HD_ERR_NONE = 0,
	HD_ERR_INVALID_ARGUMENT,
	HD_ERR_UNINITIALIZED,
} hderr_t;

#define HD_ERR_SUCCESS(_ERR_) ((_ERR_) == HD_ERR_NONE)

typedef struct _hdcont_t {
	uint16_t flag;
	uint8_t current_column;
	uint8_t current_row;
	bool cursor_blink;
	bool cursor_show;
	uint8_t dimension_column;
	uint8_t dimension_row;
	volatile uint8_t *ddr_control;
	volatile uint8_t *ddr_data;
	bool display_show;
	volatile uint8_t *port_control;
	volatile uint8_t *port_data;
	uint8_t pin_control_direction;
	uint8_t pin_control_enable;
	uint8_t pin_control_select;
} hdcont_t;

/**
 * Setup routines
 */

#define hd44780_initialize(_CONT_, _COL_, _ROW_, _INTER_, _FONT_, _SHFT_, _DATA_, \
		_CTRL_, _RS_, _RW_, _E_) \
	_hd44780_initialize(_CONT_, _COL_, _ROW_, _INTER_, _FONT_, _SHFT_, \
	&DEFINE_DDR(_DATA_), &DEFINE_PORT(_DATA_), &DEFINE_DDR(_CTRL_), \
	&DEFINE_PORT(_CTRL_), DEFINE_PIN(_CTRL_, _RS_), DEFINE_PIN(_CTRL_, _RW_), \
	DEFINE_PIN(_CTRL_, _E_))

hderr_t _hd44780_initialize(
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
 * Cursor control routines
 */

hderr_t hd44780_cursor(
	__in hdcont_t *context,
	__in bool show,
	__in bool blink
	);

hderr_t hd44780_cursor_home(
	__in hdcont_t *context
	);

hderr_t hd4480_cursor_set(
	__in hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row
	);

/**
 * Display control routines
 */

hderr_t hd44780_display(
	__in hdcont_t *context,
	__in bool show
	);

hderr_t hd44780_display_clear(
	__in hdcont_t *context
	);

hderr_t hd44780_display_put(
	__in hdcont_t *context,
	__in char input
	);

/**
 * Raw device routines
 */

hderr_t hd44780_command(
	__in hdcont_t *context,
	__in uint8_t select,
	__in uint8_t direction,
	__in uint8_t data
	);

const char *hd44780_version(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HD44780_H_
