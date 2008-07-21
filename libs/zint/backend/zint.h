/*  zint.h - definitions for libzint

    libzint - the open source barcode library
    Copyright (C) 2008 Robin Stuart <zint@hotmail.co.uk>

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
#ifndef ZINT_H
#define ZINT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VERSION "1.6.1"

struct zint_symbol {
	int symbology;
	int height;
	int whitespace_width;
	int border_width;
	int output_options;
	char fgcolour[10];
	char bgcolour[10];
	char outfile[256];
	int option_1;
	int option_2;
	int option_3;
	char text[100];
	int rows;
	int width;
	char primary[100];
	char encoded_data[155][1000];
	int row_height[155];
	char errtxt[100];
};

#define BARCODE_CODE11		1
#define BARCODE_C25MATRIX	2
#define BARCODE_C25INTER	3
#define BARCODE_C25IATA		4
#define BARCODE_C25LOGIC	6
#define BARCODE_C25IND		7
#define BARCODE_CODE39		8
#define BARCODE_EXCODE39	9
#define BARCODE_EANX		13
#define BARCODE_EAN128		16
#define BARCODE_CODABAR		18
#define BARCODE_CODE128		20
#define BARCODE_DPLEIT		21
#define BARCODE_DPIDENT		22
#define BARCODE_CODE16K		23
#define BARCODE_CODE93		25
#define BARCODE_FLAT		28
#define BARCODE_RSS14		29
#define BARCODE_RSS_LTD		30
#define BARCODE_RSS_EXP		31
#define BARCODE_TELEPEN		32
#define BARCODE_UPCA		34
#define BARCODE_UPCE		37
#define BARCODE_POSTNET		40
#define BARCODE_MSI_PLESSEY	47
#define BARCODE_FIM		49
#define BARCODE_LOGMARS		50
#define BARCODE_PHARMA		51
#define BARCODE_PZN		52
#define BARCODE_PHARMA_TWO	53
#define BARCODE_PDF417		55
#define BARCODE_PDF417TRUNC	56
#define BARCODE_MAXICODE	57
#define BARCODE_QRCODE		58
#define BARCODE_CODE128B	60
#define BARCODE_AUSPOST		63
#define BARCODE_AUSREPLY	66
#define BARCODE_AUSROUTE	67
#define BARCODE_AUSREDIRECT	68
#define BARCODE_ISBNX		69
#define BARCODE_RM4SCC		70
#define BARCODE_DATAMATRIX	71
#define BARCODE_ITF14		72
#define BARCODE_NVE18		75
#define BARCODE_KOREAPOST	77
#define BARCODE_RSS14STACK	79
#define BARCODE_RSS14STACK_OMNI	80
#define BARCODE_RSS_EXPSTACK	81
#define BARCODE_PLANET		82
#define BARCODE_MICROPDF417	84
#define BARCODE_ONECODE		85
#define BARCODE_PLESSEY		86
#define BARCODE_TELEPEN_NUM	100
#define BARCODE_MSI_10		101
#define BARCODE_MSI_10_10	102
#define BARCODE_MSI_11		103
#define BARCODE_MSI_11_10	104
#define BARCODE_CODE39_43	105
#define BARCODE_EXCODE39_43	106
#define BARCODE_AZTEC		107
#define BARCODE_SUPER		108
#define BARCODE_ULTRA		109
#define BARCODE_EANX_CC		110
#define BARCODE_EAN128_CC	111
#define BARCODE_RSS14_CC	112
#define BARCODE_RSS_LTD_CC	113
#define BARCODE_RSS_EXP_CC	114
#define BARCODE_UPCA_CC		115
#define BARCODE_UPCE_CC		116
#define BARCODE_RSS14STACK_CC	117
#define BARCODE_RSS14_OMNI_CC	118
#define BARCODE_RSS_EXPSTACK_CC	119

#define BARCODE_NO_ASCII	1
#define BARCODE_BIND		2
#define BARCODE_BOX		4

#define WARN_INVALID_OPTION	2
#define ERROR_TOO_LONG		5
#define ERROR_INVALID_DATA	6
#define ERROR_INVALID_CHECK	7
#define ERROR_INVALID_OPTION	8
#define ERROR_ENCODING_PROBLEM	9
#define ERROR_FILE_ACCESS	10
#define ERROR_MEMORY		11

extern struct zint_symbol *ZBarcode_Create();
extern int ZBarcode_Delete(struct zint_symbol *symbol);
extern int ZBarcode_Encode(struct zint_symbol *symbol, unsigned char *input);
extern int ZBarcode_Print(struct zint_symbol *symbol);
extern int ZBarcode_Encode_and_Print(struct zint_symbol *symbol, unsigned char *input);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
