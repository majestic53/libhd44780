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
#include "../lib/include/hd44780.h"

#define PIN_CTRL_E 2 // D2
#define PIN_CTRL_RS 4 // D4
#define PIN_CTRL_RW 3 // D3
#define PORT_DATA B // PORTB
#define PORT_CTRL D // PORTD

int 
main(void)
{
	hdcont_t cont;
	hderr_t result = HD_ERR_NONE;

	memset(&cont, 0, sizeof(hdcont_t));

	result = hd44780_init(&cont, PORT_DATA, PORT_CTRL, PIN_CTRL_RS, 
			PIN_CTRL_RW, PIN_CTRL_E);

	if(!HD_ERR_SUCCESS(result)) {
		goto exit;
	}

	// TODO
	while(1) { _delay_ms(500); }
	// ---

exit:
	hd44780_uninit(&cont);

	return result;
}
