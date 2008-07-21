/* auspost.c - Handles Australia Post 4-State Barcode */

/*
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

#define GDSET 	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz #"

static char *AusNTable[10] = {"00", "01", "02", "10", "11", "12", "20", "21", "22", "30"};

static char *AusCTable[64] = {"222", "300", "301", "302", "310", "311", "312", "320", "321", "322",
	"000", "001", "002", "010", "011", "012", "020", "021", "022", "100", "101", "102", "110",
	"111", "112", "120", "121", "122", "200", "201", "202", "210", "211", "212", "220", "221",
	"023", "030", "031", "032", "033", "103", "113", "123", "130", "131", "132", "133", "203",
	"213", "223", "230", "231", "232", "233", "303", "313", "323", "330", "331", "332", "333",
	"003", "013"};

static char *AusBarTable[64] = {"000", "001", "002", "003", "010", "011", "012", "013", "020", "021",
	"022", "023", "030", "031", "032", "033", "100", "101", "102", "103", "110", "111", "112",
	"113", "120", "121", "122", "123", "130", "131", "132", "133", "200", "201", "202", "203",
	"210", "211", "212", "213", "220", "221", "222", "223", "230", "231", "232", "233", "300",
	"301", "302", "303", "310", "311", "312", "313", "320", "321", "322", "323", "330", "331",
	"332", "333"};

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "reedsol.h"

void rs_error(char data_pattern[])
{
	/* Adds Reed-Solomon error correction to auspost */

	int reader, triple_writer;
	char triple[31], inv_triple[31];
	char result[5];

	triple_writer = 0;

	for(reader = 2; reader < strlen(data_pattern); reader+= 3)
	{
		triple[triple_writer] = 0;
		switch(data_pattern[reader])
		{
			case '1': triple[triple_writer] += 16; break;
			case '2': triple[triple_writer] += 32; break;
			case '3': triple[triple_writer] += 48; break;
		}
		switch(data_pattern[reader + 1])
		{
			case '1': triple[triple_writer] += 4; break;
			case '2': triple[triple_writer] += 8; break;
			case '3': triple[triple_writer] += 12; break;
		}
		switch(data_pattern[reader + 2])
		{
			case '1': triple[triple_writer] += 1; break;
			case '2': triple[triple_writer] += 2; break;
			case '3': triple[triple_writer] += 3; break;
		}
		triple_writer++;

	}

	for(reader = 0; reader < triple_writer; reader++)
	{
		inv_triple[reader] = triple[(triple_writer - 1) - reader];
	}

	rs_init_gf(0x43);
	rs_init_code(4, 1);
	rs_encode(triple_writer, inv_triple, result);

	for(reader = 4; reader > 0; reader--)
	{
		concat(data_pattern, AusBarTable[result[reader - 1]]);
	}
}

int australia_post(struct zint_symbol *symbol, unsigned char source[])
{
	/* Handles Australia Posts's 4 State Codes */
	/* Customer Standard Barcode, Barcode 2 or Barcode 3 system determined automatically
	   (i.e. the FCC doesn't need to be specified by the user) dependent
	   on the length of the input string */

	/* The contents of data_pattern conform to the following standard:
	   0 = Tracker, Ascender and Descender
	   1 = Tracker and Ascender
	   2 = Tracker and Descender
	   3 = Tracker only */

	char data_pattern[200];
	char fcc[3], dpid[10];
	unsigned int loopey, reader;
	int writer;
	strcpy (data_pattern, "");
	int errno;

	errno = 0;
	
	/* Do all of the length checking first to avoid stack smashing */
	if(symbol->symbology == BARCODE_AUSPOST) {
		/* Format control code (FCC) */
		switch(strlen(source))
		{
			case 8: strcpy(fcc, "11"); break;
			case 13: strcpy(fcc, "59"); break;
			case 16: strcpy(fcc, "59"); errno = is_sane(NESET, source); break;
			case 18: strcpy(fcc, "62"); break;
			case 23: strcpy(fcc, "62"); errno = is_sane(NESET, source); break;
			default: strcpy(symbol->errtxt, "error: auspost input is wrong length");
			return ERROR_TOO_LONG;
				break;
		}
		if(errno == ERROR_INVALID_DATA) {
			strcpy(symbol->errtxt, "error: invalid characters in data");
			return errno;
		}
	} else {
		if(strlen(source) != 8) {
			strcpy(symbol->errtxt, "error: auspost input is wrong length");
			return ERROR_TOO_LONG;
		}
		switch(symbol->symbology) {
			case BARCODE_AUSREPLY: strcpy(fcc, "45"); break;
			case BARCODE_AUSROUTE: strcpy(fcc, "87"); break;
			case BARCODE_AUSREDIRECT: strcpy(fcc, "92"); break;
		}
	}

	
	errno = is_sane(GDSET, source);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in data");
		return errno;
	}
	
	/* Verifiy that the first 8 characters are numbers */
	for(loopey = 0; loopey < 8; loopey++) {
		dpid[loopey] = source[loopey];
	}
	dpid[8] = '\0';
	errno = is_sane(NESET, dpid);
	if(errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: invalid characters in DPID");
		return errno;
	}

	/* Start character */
	concat(data_pattern, "13");

	/* Encode the FCC */
	for(reader = 0; reader < 2; reader++)
	{
		lookup(NESET, AusNTable, fcc[reader], data_pattern);
	}

	/* printf("AUSPOST FCC: %s  ", fcc); */

	/* Delivery Point Identifier (DPID) */
	for(reader = 0; reader < 8; reader++)
	{
		lookup(NESET, AusNTable, dpid[reader], data_pattern);
	}

	/* Customer Information */
	if(strlen(source) > 8)
	{
		if((strlen(source) == 13) || (strlen(source) == 18)) {
			for(reader = 8; reader < strlen(source); reader++) {
				lookup(GDSET, AusCTable, source[reader], data_pattern);
			}
		}
		if((strlen(source) == 16) || (strlen(source) == 23)) {
			for(reader = 8; reader < strlen(source); reader++) {
				lookup(NESET, AusNTable, source[reader], data_pattern);
			}
		}
	}

	/* Filler bar */
	if(strlen(data_pattern) == 22) {
		concat(data_pattern, "3");
	}
	if(strlen(data_pattern) == 37) {
		concat(data_pattern, "3");
	}
	if(strlen(data_pattern) == 52) {
		concat(data_pattern, "3");
	}

	/* Reed Solomon error correction */
	rs_error(data_pattern);

	/* Stop character */
	concat(data_pattern, "13");
	
	/* Turn the symbol into a bar pattern ready for plotting */
	writer = 0;
	for(loopey = 0; loopey < strlen(data_pattern); loopey++)
	{
		if((data_pattern[loopey] == '1') || (data_pattern[loopey] == '0'))
		{
			symbol->encoded_data[0][writer] = '1';
		}
		symbol->encoded_data[1][writer] = '1';
		if((data_pattern[loopey] == '2') || (data_pattern[loopey] == '0'))
		{
			symbol->encoded_data[2][writer] = '1';
		}
		writer += 2;
	}

	symbol->row_height[0] = 4;
	symbol->row_height[1] = 2;
	symbol->row_height[2] = 4;

	symbol->rows = 3;
	symbol->width = writer - 1;

	return errno;
}

