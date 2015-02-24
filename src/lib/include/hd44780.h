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

#define DEF_DDR(_BNK_) DDR ## _BNK_
#define DEF_PIN(_BNK_, _PIN_) P ## _BNK_ ## _PIN_
#define DEF_PORT(_BNK_) PORT ## _BNK_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum _hderr_t {
	HD_ERR_NONE = 0,
	HD_ERR_INVALID,
	HD_ERR_UNINIT,
} hderr_t;

#define HD_ERR_SUCCESS(_ERR_) ((_ERR_) == HD_ERR_NONE)

typedef struct _hdcont_t {
	uint16_t init;
	volatile uint8_t *ddr_ctrl;
	volatile uint8_t *ddr_data;
	volatile uint8_t *port_ctrl;
	volatile uint8_t *port_data;
	uint8_t pin_ctrl_e;
	uint8_t pin_ctrl_rs;
	uint8_t pin_ctrl_rw;
} hdcont_t;

hderr_t hd44780_command(
	__in hdcont_t *cont,
	__in uint8_t rs,
	__in uint8_t rw,
	__in uint8_t data
	);

#define hd44780_init(_CONT_, _DATA_, _CTRL_, _RS_, _RW_, _E_) \
	_hd44780_init(_CONT_, &DEF_DDR(_DATA_), &DEF_PORT(_DATA_), \
	&DEF_DDR(_CTRL_), &DEF_PORT(_CTRL_), DEF_PIN(_CTRL_, _RS_), \
	DEF_PIN(_CTRL_, _RW_), DEF_PIN(_CTRL_, _E_))

hderr_t _hd44780_init(
	__out hdcont_t *cont,
	__in volatile uint8_t *ddr_data,
	__in volatile uint8_t *port_data,
	__in volatile uint8_t *ddr_ctrl,
	__in volatile uint8_t *port_ctrl,
	__in uint8_t pin_ctrl_rs,
	__in uint8_t pin_ctrl_rw,
	__in uint8_t pin_ctrl_e
	);

void hd44780_uninit(
	__out hdcont_t *cont
	);

const char *hd44780_version(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HD44780_H_
