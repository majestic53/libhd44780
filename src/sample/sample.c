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
#include <avr/io.h>
#include "../lib/include/hd44780.h"

#define PORT_DATA B
#define PORT_CTRL C
#define PIN_ENB 0
#define PIN_SEL 1
#define PIN_DIR 2

int 
main(void)
{
	hdcont_t cont;
	hderr_t result = HD_ERR_NONE;

	memset(&cont, 0, sizeof(hdcont_t));

	result = hd44780_init(&cont, PORT_DATA, PORT_CTRL, PIN_ENB, PIN_SEL, PIN_DIR);
	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	// TODO

exit:
	hd44780_uninit(&cont);

	// TODO: REMOVE AFTER DEBUG
	DDRC |= _BV(PC0);
	PORTC |= _BV(PC0);
	// ---

	return result;
}
