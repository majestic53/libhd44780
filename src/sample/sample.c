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

#define DISPLAY_COL 16
#define DISPLAY_ROW 2

#define PIN_CTRL_E 2 // D2
#define PIN_CTRL_RS 4 // D4
#define PIN_CTRL_RW 3 // D3
#define PORT_DATA B // PORTB
#define PORT_CTRL D // PORTD

#define BAUD 9600
#include <util/setbaud.h>

inline void
uart_initialize(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif // USE_2X
	UCSR0B = _BV(RXEN0);
	UCSR0C = (_BV(UCSZ01) | _BV(UCSZ00));
}

inline char 
uart_read(void)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

int 
main(void)
{
	hdcont_t cont;

	memset(&cont, 0, sizeof(hdcont_t));

	hd44780_initialize(&cont, DISPLAY_COL, DISPLAY_ROW, INTERFACE_8_BIT, FONT_EN_JP, 
			SHIFT_RIGHT, PORT_DATA, PORT_CTRL, PIN_CTRL_RS, PIN_CTRL_RW, PIN_CTRL_E);

	uart_initialize();
	hd44780_cursor(&cont, CURSOR_ON, CURSOR_BLINK_ON);

	while(1) {
		hd44780_display_putc(&cont, uart_read());
	}

	hd44780_uninitialize(&cont);

	return 0;
}
