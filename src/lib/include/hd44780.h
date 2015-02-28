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

#define DIMENSION_TYPE_MAX DIMENSION_40_2

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

/**
 * Direction register name macro
 * Allows the caller to specify a direction register by name
 * @param _BNK_ register alphabetic name
 */
#define DEFINE_DDR(_BNK_) DDR ## _BNK_

/**
 * Pin name macro
 * Allows the caller to specify a pin on a specified register by name
 * @param _BNK_ register alphabetic name
 * @param _PIN_ pin numeric name
 */
#define DEFINE_PIN(_BNK_, _PIN_) P ## _BNK_ ## _PIN_

/**
 * Port register name macro
 * Allows the caller to specify a port register by name
 * @param _BNK_ register alphabetic name
 */
#define DEFINE_PORT(_BNK_) PORT ## _BNK_

/**
 * Holds pin/port configuration information
 */
typedef struct _hdcont_comm_t {
	volatile uint8_t *ddr_control;		// control port direction
	volatile uint8_t *ddr_data;		// data port direction
	uint8_t pin_control_direction;		// direction pin
	uint8_t pin_control_enable;		// enable pin
	uint8_t pin_control_select;		// select pin
	volatile uint8_t *port_control;		// control port
	volatile uint8_t *port_data;		// data port
} hdcont_comm_t;

/**
 * Holds cursor/display state information
 */
typedef struct _hdcont_state_t {
	uint8_t current_column;			// cursor column
	uint8_t current_row;			// cursor row
	uint8_t cursor_blink;			// show cursor blink flag		
	uint8_t cursor_show;			// show cursor flag
	uint8_t dimension_column;		// display column count
	uint8_t dimension_row;			// display row count
	uint8_t display_show;			// show display flag
} hdcont_state_t;

/**
 * Holds device context information
 */
typedef struct _hdcont_t {
	uint8_t dimension;			// dimension type
	uint8_t interface;			// interface type
	hdcont_comm_t comm;			// pin/port connections
	hdcont_state_t state;			// cursor/display state
} hdcont_t;

/***********************************************************************************
 * ** Setup routines **
 * These routines must be called during initialize and uninitialize
 ***********************************************************************************/

/**
 * Device initialization macro
 * This macro must be called prior to any other device calls
 * @param _CONT_ caller supplied device context pointer
 * @param _DIM_ device dimension type
 * @param _INTER_ device interface type
 * @param _FONT_ device font table type
 * @param _DATA_ device data port
 * @param _CTRL_ device control port
 * @param _SEL_ device select pin
 * @param _DIR_ device direction pin
 * @param _E_ device enable pin
 */
#define hd44780_initialize(_CONT_, _DIM_, _INTER_, _FONT_, _DATA_, _CTRL_, _SEL_, \
		_DIR_, _E_) \
	_hd44780_initialize(_CONT_, _DIM_, _INTER_, _FONT_, &DEFINE_DDR(_DATA_), \
	&DEFINE_PORT(_DATA_), &DEFINE_DDR(_CTRL_), &DEFINE_PORT(_CTRL_), \
	DEFINE_PIN(_CTRL_, _SEL_), DEFINE_PIN(_CTRL_, _DIR_), DEFINE_PIN(_CTRL_, _E_))
void _hd44780_initialize(
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
	);

/**
 * Device uninitialization routine
 * This routine must be called after all other device calls
 * @param context caller supplied device context pointer
 */
void hd44780_uninitialize(
	__out hdcont_t *context
	);

/***********************************************************************************
 * ** Cursor routines **
 * These routines manipulate a devices cursor state
 ***********************************************************************************/

/**
 * Cursor blink flags
 */
#define CURSOR_BLINK_OFF 0
#define CURSOR_BLINK_ON 1

/**
 * Cursor show flags
 */
#define CURSOR_OFF 0
#define CURSOR_ON 1

/**
 * Cursor configuration routine
 * Allows the caller to configure the cursor state of a specified device context
 * @param context caller supplied device context pointer
 * @param show cursor show flag (0: OFF, >0: ON)
 * @param blink cursor blink flag (0: NO BLINK, 1: BLINK)
 */
void hd44780_cursor(
	__in hdcont_t *context,
	__in uint8_t show,
	__in uint8_t blink
	);

/**
 * Cursor home routine
 * Allows the caller to home (reset) the cursor position to (0, 0) of a specified 
 *  device context
 * @param context caller supplied device context pointer
 */
void hd44780_cursor_home(
	__in hdcont_t *context
	);

/**
 * Cursor set routine
 * Allows the caller to set the cursor position (col, row) of a specified device 
 *   context
 * @param context caller supplied device context pointer
 * @param column cursor column
 * @param row cursor row
 */
void hd4480_cursor_set(
	__in hdcont_t *context,
	__in uint8_t column,
	__in uint8_t row
	);

/***********************************************************************************
 * ** Display routines **
 * These routines manipulate a devices display state
 ***********************************************************************************/

/**
 * Display show flags
 */
#define DISPLAY_OFF 0
#define DISPLAY_ON 1

/**
 * Display configuration routine
 * Allows the caller to configure the display state of a specified device context
 * @param context caller supplied device context pointer
 * @param show display show flag (0: OFF, >0: ON)
 */
void hd44780_display(
	__in hdcont_t *context,
	__in uint8_t show
	);

/**
 * Display clear routine
 * Allows the caller to clear the display of a specified device context
 * @param context caller supplied device context pointer
 */
void hd44780_display_clear(
	__in hdcont_t *context
	);

/**
 * Display character routine
 * Allows the caller to place a character onto the display of a specified 
 *   device context
 * @param context caller supplied device context pointer
 * @param input character
 */
void hd44780_display_putc(
	__in hdcont_t *context,
	__in char input
	);

/**
 * Display string routine
 * Allows the caller to place a string onto the display of a specified 
 *   device context
 * @param context caller supplied device context pointer
 * @param input caller supplied character pointer
 */
void hd44780_display_puts(
	__in hdcont_t *context,
	__in char *input
	);

/***********************************************************************************
 * ** Device routines **
 * These routines allow lower-level device access
 ***********************************************************************************/

/**
 * Device command routine
 * Allows the caller to issue a raw device command
 * @param context caller supplied device context pointer
 * @param select select control pin value
 * @param direction direction control pin value
 * @param data data data value
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
