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

//#define NDEBUG

#include <stdbool.h>
#include <stdint.h>

#ifndef __in
#define __in
#endif // __in
#ifndef __out
#define __out
#endif // __out

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define DEF_DDR(_BNK_) DDR ## _BNK_
#define DEF_PORT(_BNK_) PORT ## _BNK_
#define DEF_PIN(_BNK_, _PIN_) P ## _BNK_ ## _PIN_

typedef enum hderr_t {
	HD_ERR_NONE = 0,
	HD_ERR_INVALID,
	HD_ERR_UNINIT,
} hderr_t;

#define HD_ERR_SUCCESS(_ERR_) ((_ERR_) == HD_ERR_NONE)

typedef struct _hdcont_ctrl_t {
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	uint8_t pin_dir;
	uint8_t pin_enab;
	uint8_t pin_sel;
} hdcont_ctrl_t;

typedef struct _hdcont_data_t {
	volatile uint8_t *ddr;
	volatile uint8_t *port;
} hdcont_data_t;

typedef struct _hdcont_t {
	uint16_t init;
	hdcont_ctrl_t ctrl;
	hdcont_data_t data;
} hdcont_t;

uint16_t hd44780_version(void);

/**
 * Initialization/Uninitialization routines
 */

#define hd44780_init(_CONT_, _DATA_, _CTRL_, _ENB_, _SEL_, _DIR_) \
	_hd44780_init(_CONT_, &DEF_DDR(_DATA_), &DEF_PORT(_DATA_), &DEF_DDR(_CTRL_), \
	&DEF_PORT(_CTRL_), DEF_PIN(_CTRL_, _ENB_), DEF_PIN(_CTRL_, _SEL_), \
	DEF_PIN(_CTRL_, _DIR_))

hderr_t _hd44780_init(
	__out hdcont_t *cont,
	__in volatile uint8_t *ddr_data,
	__in volatile uint8_t *port_data,
	__in volatile uint8_t *ddr_ctrl,
	__in volatile uint8_t *port_ctrl,
	__in uint8_t pin_enab,
	__in uint8_t pin_sel,
	__in uint8_t pin_dir
	);

void hd44780_uninit(
	__in hdcont_t *cont
	);

/**
 * Cursor manipulation routines
 */

hderr_t hd44780_cursor_blink(
	__out hdcont_t *cont,
	__in bool set
	);

hderr_t hd44780_cursor_home(
	__out hdcont_t *cont
	);

hderr_t hd44780_cursor_left(
	__out hdcont_t *cont
	);

hderr_t hd44780_cursor_right(
	__out hdcont_t *cont
	);

hderr_t hd44780_cursor_set(
	__out hdcont_t *cont,
	__in uint8_t row,
	__in uint8_t col
	);

hderr_t hd44780_cursor_show(
	__out hdcont_t *cont,
	__in bool set
	);

/**
 * Screen manipulation routines
 */

hderr_t hd44780_screen_clear(
	__out hdcont_t *cont
	);

hderr_t hd44780_screen_left(
	__out hdcont_t *cont
	);

hderr_t hd44780_screen_right(
	__out hdcont_t *cont
	);

/**
 * Screen IO routines
 */

hderr_t hd44780_write_char(
	__out hdcont_t *cont,
	__in char ch
	);

hderr_t hd44780_write_str(
	__out hdcont_t *cont,
	__in const char *str,
	__in uint16_t len
	);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HD44780_H_
