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

#ifndef __in
#define __in
#endif // __in
#ifndef __out
#define __out
#endif // __out

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

uint16_t hd44780_api_version(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HD44780_H_
