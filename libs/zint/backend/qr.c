/* qr.c Handles QR Code by utilising libqrencode */

/*
    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>
    Copyright (C) 2006 Kentaro Fukuchi <fukuchi@megaui.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <string.h>
#include <qrencode.h> /* The assumption is that this is already installed */
#include "common.h"

static int kanji = 0;
static QRecLevel level = QR_ECLEVEL_L;

QRcode *encode(int security, int size, const unsigned char *intext)
{
	int version;
	QRecLevel level;
	QRencodeMode hint;
	QRcode *code;

	if(kanji) {
		hint = QR_MODE_KANJI;
	} else {
		hint = QR_MODE_8;
	}

	if((security >= 1) && (security <= 4)) {
		switch (security) {
			case 1: level = QR_ECLEVEL_L; break;
			case 2: level = QR_ECLEVEL_M; break;
			case 3: level = QR_ECLEVEL_Q; break;
			case 4: level = QR_ECLEVEL_H; break;
		}
	} else {
		level = QR_ECLEVEL_L;
	}
	
	if((size >= 1) && (size <= 40)) {
		version = size;
	} else {
		version = 0;
	}

	code = QRcode_encodeString(intext, version, level, hint, 1);

	return code;
}

int qr_code(struct zint_symbol *symbol, unsigned char source[])
{
	QRcode *code;
	int errno = 0;
	int i, j;
	
	code = encode(symbol->option_1, symbol->option_2, source);
	if(code == NULL) {
		strcpy(symbol->errtxt, "error: libqrencode failed to encode the input data");
		return ERROR_ENCODING_PROBLEM;
	}
	
	symbol->width = code->width;
	symbol->rows = code->width;
	
	for(i = 0; i < code->width; i++) {
		for(j = 0; j < code->width; j++) {
			if((*(code->data + (i * code->width) + j) & 0x01) == 0) {
				symbol->encoded_data[i][j] = '0';
			} else {
				symbol->encoded_data[i][j] = '1';
			}
		}
		symbol->row_height[i] = 1;
	}
	
	QRcode_free(code);
	
	return errno;
}
