/* png.c - Handles output to PNG file */

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "png.h"        /* libpng header; includes zlib.h and setjmp.h */
#include "common.h"
#include "maxipng.h"	/* Maxicode shapes */
#include "font.h"	/* Font for human readable text */

#define SSET	"0123456789ABCDEF"

struct mainprog_info_type {
    long width;
    long height;
    FILE *outfile;
    jmp_buf jmpbuf;
};

static void writepng_error_handler(png_structp png_ptr, png_const_charp msg)
{
    struct mainprog_info_type  *graphic;

    fprintf(stderr, "writepng libpng error: %s\n", msg);
    fflush(stderr);

    graphic = png_get_error_ptr(png_ptr);
    if (graphic == NULL) {         /* we are completely hosed now */
        fprintf(stderr,
          "writepng severe error:  jmpbuf not recoverable; terminating.\n");
        fflush(stderr);
        exit(99);
    }
    longjmp(graphic->jmpbuf, 1);
}

int png_to_file(struct zint_symbol *symbol, int image_height, int image_width, char *pixelbuf)
{
#ifndef NO_PNG
	struct mainprog_info_type wpng_info;
	struct mainprog_info_type *graphic;
	unsigned char outdata[image_width * 3];
	png_structp  png_ptr;
	png_infop  info_ptr;
	graphic = &wpng_info;
	long j;
	unsigned long rowbytes;
	unsigned char *image_data;
	int i, k, offset, row, column, errno;
	int fgred, fggrn, fgblu, bgred, bggrn, bgblu;
	
	graphic->width = image_width;
	graphic->height = image_height;
	
	/* sort out colour options */
	to_upper(symbol->fgcolour);
	to_upper(symbol->bgcolour);
	
	if(strlen(symbol->fgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	if(strlen(symbol->bgcolour) != 6) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->fgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed foreground colour target");
		return ERROR_INVALID_OPTION;
	}
	errno = is_sane(SSET, symbol->bgcolour);
	if (errno == ERROR_INVALID_DATA) {
		strcpy(symbol->errtxt, "error: malformed background colour target");
		return ERROR_INVALID_OPTION;
	}
	
	fgred = (16 * ctoi(symbol->fgcolour[0])) + ctoi(symbol->fgcolour[1]);
	fggrn = (16 * ctoi(symbol->fgcolour[2])) + ctoi(symbol->fgcolour[3]);
	fgblu = (16 * ctoi(symbol->fgcolour[4])) + ctoi(symbol->fgcolour[5]);
	bgred = (16 * ctoi(symbol->bgcolour[0])) + ctoi(symbol->bgcolour[1]);
	bggrn = (16 * ctoi(symbol->bgcolour[2])) + ctoi(symbol->bgcolour[3]);
	bgblu = (16 * ctoi(symbol->bgcolour[4])) + ctoi(symbol->bgcolour[5]);
	
	/* Open output file in binary mode */
	if (!(graphic->outfile = fopen(symbol->outfile, "wb"))) {
		strcpy(symbol->errtxt, "error: can't open output file");
		return ERROR_FILE_ACCESS;
	}
	
	/* Set up error handling routine as proc() above */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, graphic,
					  writepng_error_handler, NULL);
	if (!png_ptr) {
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, NULL);
		strcpy(symbol->errtxt, "error: out of memory");
		return ERROR_MEMORY;
	}

	/* catch jumping here */
	if (setjmp(graphic->jmpbuf)) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		strcpy(symbol->errtxt, "error: libpng error occurred");
		return ERROR_MEMORY;
	}

	/* open output file with libpng */
	png_init_io(png_ptr, graphic->outfile);

	/* set compression */
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	/* set Header block */
	png_set_IHDR(png_ptr, info_ptr, graphic->width, graphic->height,
		     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	/* write all chunks up to (but not including) first IDAT */
	png_write_info(png_ptr, info_ptr);

	/* set up the transformations:  for now, just pack low-bit-depth pixels
	into bytes (one, two or four pixels per byte) */
	png_set_packing(png_ptr);

	/* set rowbytes - depends on picture depth */
	rowbytes = wpng_info.width * 3;

	/* Pixel Plotting */

	for(row = 0; row < image_height; row++) {
		for(column = 0; column < image_width; column++) {
			i = column * 3;
			switch(*(pixelbuf + (image_width * row) + column))
			{
				case '1':
					outdata[i] = fgred;
					outdata[i + 1] = fggrn;
					outdata[i + 2] = fgblu;
					break;
				default:
					outdata[i] = bgred;
					outdata[i + 1] = bggrn;
					outdata[i + 2] = bgblu;
					break;
		
			}
		}
	
		/* write row contents to file */
		image_data = outdata;
		png_write_row(png_ptr, image_data);
	}

	/* End the file */
	png_write_end(png_ptr, NULL);

	/* make sure we have disengaged */
	if (png_ptr && info_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(wpng_info.outfile);
#endif
	return 0;
}

void draw_bar(char *pixelbuf, int xpos, int xlen, int ypos, int ylen, int image_width, int image_height)
{
	/* Draw a rectangle */
	int i, j, png_ypos;
	
	png_ypos = (image_height / 2) - ypos - ylen;
	/* This fudge is needed because EPS measures height from the bottom up but
	PNG measures y position from the top down */
	
	for(i = (xpos * 2); i < (2 * (xpos + xlen)); i++) {
		for( j = (png_ypos * 2); j < (2 * (png_ypos + ylen)); j++) {
			*(pixelbuf + (image_width * j) + i) = '1';
		}
	}
}

void draw_bullseye(char *pixelbuf)
{
	/* Central bullseye in Maxicode symbols */
	int i, j;
	
	for(j = 103; j < 196; j++) {
		for(i = 0; i < 93; i++) {
			if(bullseye[(((j - 103) * 93) + i)] == 1) {
				*(pixelbuf + (300 * j) + (i + 99)) = '1';
			}
		}
	}
}

void draw_hexagon(char *pixelbuf, int xposn, int yposn)
{
	/* Put a hexagon into the pixel buffer */
	int i, j;
	
	for(i = 0; i < 12; i++) {
		for(j = 0; j < 10; j++) {
			if(hexagon[(i * 10) + j] == 1) {
				*(pixelbuf + (300 * i) + (300 * yposn) + xposn + j) = '1';
			}
		}
	}
}

void draw_letter(char *pixelbuf, char letter, int xposn, int yposn, int image_width, int image_height)
{
	/* Put a letter into a position */
	int skip, i, j, glyph_no, alphabet;
	
	skip = 0;
	alphabet = 0;
	
	if(letter < 33) { skip = 1; }
	if((letter > 127) && (letter < 161)) { skip = 1; }
	
	if(skip == 0) {
		if(letter > 128) {
			alphabet = 1;
			glyph_no = letter - 161;
		} else {
			glyph_no = letter - 33;
		}
		
		for(i = 0; i < 13; i++) {
			for(j = 0; j < 7; j++) {
				if(alphabet == 0) {
					if(ascii_font[(glyph_no * 7) + (i * 665) + j - 1] == 1) {
						*(pixelbuf + (i * image_width) + (yposn * image_width) + xposn + j) = '1';
					}
				} else {
					if(ascii_ext_font[(glyph_no * 7) + (i * 665) + j - 1] == 1) {
						*(pixelbuf + (i * image_width) + (yposn * image_width) + xposn + j) = '1';
					}
				}
			}
		}
	}
}

void draw_string(char *pixbuf, char input_string[], int xposn, int yposn, int image_width, int image_height)
{
	/* Plot a string into the pixel buffer */
	int i, string_length, string_left_hand;
	
	string_length = strlen(input_string);
	string_left_hand = xposn - ((7 * string_length) / 2);
	
	for(i = 0; i < string_length; i++) {
		draw_letter(pixbuf, input_string[i], string_left_hand + (i * 7), yposn, image_width, image_height);
	}
}

int maxi_png_plot(struct zint_symbol *symbol)
{
	int i, row, column, xposn, yposn;
	int image_height, image_width;
	char *pixelbuf;
	
	image_width = 300;
	image_height = 300;
	
	if (!(pixelbuf = (char *) malloc(image_width * image_height))) {
		printf("Insifficient memory for pixel buffer");
		return ERROR_ENCODING_PROBLEM;
	} else {
		for(i = 0; i < (image_width * image_height); i++) {
			*(pixelbuf + i) = '0';
		}
	}
	
	draw_bullseye(pixelbuf);
	
	for(row = 0; row < symbol->rows; row++) {
		yposn = row * 9;
		for(column = 0; column < symbol->width; column++) {
			xposn = column * 10;
			if(symbol->encoded_data[row][column] == '1') {
				if((row % 2) == 0) {
					/* Even (full) row */
					draw_hexagon(pixelbuf, xposn, yposn);
				} else {
					/* Odd (reduced) row */
					xposn += 5;
					draw_hexagon(pixelbuf, xposn, yposn);
				}
			}
		}
	}

	png_to_file(symbol, image_height, image_width, pixelbuf);
}

int png_plot(struct zint_symbol *symbol)
{
	int textdone, main_width, comp_offset, large_bar_count;
	char textpart[10], addon[6];
	float addon_text_posn, preset_height, large_bar_height;
	int i, r, textoffset, yoffset, xoffset, latch, image_width, image_height;
	char *pixelbuf;
	int addon_latch = 0;
	int this_row, block_width, plot_height, plot_yposn, textpos;
	float row_height, row_posn;
	
	textdone = 0;
	main_width = symbol->width;
	strcpy(addon, "");
	comp_offset = 0;
	addon_text_posn = 0.0;
	
	if (symbol->height == 0) {
		symbol->height = 50;
	}
	
	large_bar_count = 0;
	preset_height = 0.0;
	for(i = 0; i < symbol->rows; i++) {
		preset_height += symbol->row_height[i];
		if(symbol->row_height[i] == 0) {
			large_bar_count++;
		}
	}
	large_bar_height = (symbol->height - preset_height) / large_bar_count;

	if (large_bar_count == 0) {
		symbol->height = preset_height;
	}
	
	while(symbol->encoded_data[symbol->rows - 1][comp_offset] != '1') {
		comp_offset++;
	}

	/* Certain symbols need whitespace otherwise characters get chopped off the sides */
	if (((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) {
		switch(strlen(symbol->text)) {
			case 13: /* EAN 13 */
			case 16:
			case 19:
				if(symbol->whitespace_width == 0) {
					symbol->whitespace_width = 10;
				}
				main_width = 96 + comp_offset;
				break;
			default:
				main_width = 68 + comp_offset;
		}
	}
	
	if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 96 + comp_offset;
		}
	}
	
	if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
		if(symbol->whitespace_width == 0) {
			symbol->whitespace_width = 10;
			main_width = 51 + comp_offset;
		}
	}
	
	latch = 0;
	r = 0;
	/* Isolate add-on text */
	for(i = 0; i < strlen(symbol->text); i++) {
		if (latch == 1) {
			addon[r] = symbol->text[i];
			r++;
		}
		if (symbol->text[i] == '+') {
			latch = 1;
		}
	}
	addon[r] = '\0';
	
	if(strcmp(symbol->text, "")) {
		textoffset = 9;
	} else {
		textoffset = 0;
	}
	xoffset = symbol->border_width + symbol->whitespace_width;
	yoffset = symbol->border_width;
	image_width = 2 * (symbol->width + xoffset + xoffset);
	image_height = 2 * (symbol->height + textoffset + yoffset + yoffset);
	
	if (!(pixelbuf = (char *) malloc(image_width * image_height))) {
		printf("Insifficient memory for pixel buffer");
		return ERROR_ENCODING_PROBLEM;
	} else {
		for(i = 0; i < (image_width * image_height); i++) {
			*(pixelbuf + i) = '0';
		}
	}
	
	/* Plot the body of the symbol to the pixel buffer */
	for(r = 0; r < symbol->rows; r++) {
		this_row = symbol->rows - r - 1; /* invert r otherwise plots upside down */
		if(symbol->row_height[this_row] == 0) {
			row_height = large_bar_height;
		} else {
			row_height = symbol->row_height[this_row];
		}
		row_posn = 0;
		for(i = 0; i < r; i++) {
			if(symbol->row_height[symbol->rows - i - 1] == 0) {
				row_posn += large_bar_height;
			} else {
				row_posn += symbol->row_height[symbol->rows - i - 1];
			}
		}
		row_posn += (textoffset + yoffset);
		
		plot_height = (int)row_height;
		plot_yposn = (int)row_posn;
		
		i = 0;
		if(symbol->encoded_data[this_row][0] == '1') {
			latch = 1;
		} else {
			latch = 0;
		}
			
		do {
			block_width = 0;
			do {
				block_width++;
			} while (symbol->encoded_data[this_row][i + block_width] == symbol->encoded_data[this_row][i]);
			if((addon_latch == 0) && (r == 0) && (i > main_width)) {
				plot_height = (int)(row_height - 5.0);
				plot_yposn = (int)(row_posn - 5.0);
				addon_text_posn = row_posn + row_height - 8.0;
				addon_latch = 1;
			} 
			if(latch == 1) { 
				/* a bar */
				draw_bar(pixelbuf, (i + xoffset), block_width, plot_yposn, plot_height, image_width, image_height);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
				
		} while (i < symbol->width);
	}
	
	xoffset += comp_offset;

	if (((symbol->symbology == BARCODE_EANX) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_EANX_CC)) {
		/* guard bar extensions and text formatting for EAN8 and EAN13 */
		switch(strlen(symbol->text)) {
			case 8: /* EAN-8 */
			case 11:
			case 14:
				draw_bar(pixelbuf, (0 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (2 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (32 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (34 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (64 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (66 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				for(i = 0; i < 4; i++) {
					textpart[i] = symbol->text[i];
				}
				textpart[4] = '\0';
				textpos = 2 * (17 + xoffset);
				draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
				for(i = 0; i < 4; i++) {
					textpart[i] = symbol->text[i + 4];
				}
				textpart[4] = '\0';
				textpos = 2 * (50 + xoffset);
				draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						textpos = 2 * (symbol->width + xoffset - 10);
						draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
						break;
					case 5:
						textpos = 2 * (symbol->width + xoffset - 23);
						draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
						break;
				}

				break;
			case 13: /* EAN 13 */
			case 16:
			case 19:
				draw_bar(pixelbuf, (0 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (2 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (46 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (48 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (92 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
				draw_bar(pixelbuf, (94 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);

				textpart[0] = symbol->text[0];
				textpos = 2 * (-7 + xoffset);
				draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
				for(i = 0; i < 6; i++) {
					textpart[i] = symbol->text[i + 1];
				}
				textpart[6] = '\0';
				textpos = 2 * (24 + xoffset);
				draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
				for(i = 0; i < 6; i++) {
					textpart[i] = symbol->text[i + 7];
				}
				textpart[6] = '\0';
				textpos = 2 * (71 + xoffset);
				draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
				textdone = 1;
				switch(strlen(addon)) {
					case 2:	
						textpos = 2 * (symbol->width + xoffset - 10);
						draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
						break;
					case 5:
						textpos = 2 * (symbol->width + xoffset - 23);
						draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
						break;
				}
				break;

		}
	}	

	if (((symbol->symbology == BARCODE_UPCA) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCA_CC)) {
		/* guard bar extensions and text formatting for UPCA */
		latch = 1;
		
		i = 0 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (symbol->encoded_data[symbol->rows - 1][i + block_width] == symbol->encoded_data[symbol->rows - 1][i]);
			if(latch == 1) {
				/* a bar */
				draw_bar(pixelbuf, (i + xoffset - comp_offset), block_width, (4 + (int)yoffset), 5, image_width, image_height);
				/*fprintf(feps, "TB %d.00 %d.00 TR\n", i + xoffset - comp_offset, block_width);*/
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 11 + comp_offset);
		draw_bar(pixelbuf, (46 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		draw_bar(pixelbuf, (48 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		latch = 1;
		i = 85 + comp_offset;
		do {
			block_width = 0;
			do {
				block_width++;
			} while (symbol->encoded_data[symbol->rows - 1][i + block_width] == symbol->encoded_data[symbol->rows - 1][i]);
			if(latch == 1) {
				/* a bar */
				draw_bar(pixelbuf, (i + xoffset - comp_offset), block_width, (4 + (int)yoffset), 5, image_width, image_height);
				latch = 0;
			} else {
				/* a space */
				latch = 1;
			}
			i += block_width;
		} while (i < 96 + comp_offset);
		textpart[0] = symbol->text[0];
		textpart[1] = '\0';
		textpos = 2 * (-5 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		for(i = 0; i < 5; i++) {
			textpart[i] = symbol->text[i + 1];
		}
		textpart[5] = '\0';
		textpos = 2 * (27 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		for(i = 0; i < 5; i++) {
			textpart[i] = symbol->text[i + 6];
		}
		textpart[6] = '\0';
		textpos = 2 * (68 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		textpart[0] = symbol->text[11];
		textpart[1] = '\0';
		textpos = 2 * (100 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				textpos = 2 * (symbol->width + xoffset - 10);
				draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
				break;
			case 5:
				textpos = 2 * (symbol->width + xoffset - 23);
				draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
				break;
		}

	}	

	if (((symbol->symbology == BARCODE_UPCE) && (symbol->rows == 1)) || (symbol->symbology == BARCODE_UPCE_CC)) {
		/* guard bar extensions and text formatting for UPCE */
		draw_bar(pixelbuf, (0 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		draw_bar(pixelbuf, (2 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		draw_bar(pixelbuf, (46 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		draw_bar(pixelbuf, (48 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);
		draw_bar(pixelbuf, (50 + xoffset), 1, (4 + (int)yoffset), 5, image_width, image_height);

		textpart[0] = symbol->text[0];
		textpart[1] = '\0';
		textpos = 2 * (-5 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		for(i = 0; i < 6; i++) {
			textpart[i] = symbol->text[i + 1];
		}
		textpart[6] = '\0';
		textpos = 2 * (24 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		textpart[0] = symbol->text[7];
		textpart[1] = '\0';
		textpos = 2 * (55 + xoffset);
		draw_string(pixelbuf, textpart, textpos, (image_height - 17), image_width, image_height);
		textdone = 1;
		switch(strlen(addon)) {
			case 2:	
				textpos = 2 * (symbol->width + xoffset - 10);
				draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
				break;
			case 5:
				textpos = 2 * (symbol->width + xoffset - 23);
				draw_string(pixelbuf, addon, textpos, image_height - (addon_text_posn * 2) - 13, image_width, image_height);
				break;
		}

	}

	xoffset -= comp_offset;
	
	/* Put boundary bars or box around symbol */
	if ((symbol->output_options == BARCODE_BOX) || (symbol->output_options == BARCODE_BIND)) {
		/* boundary bars */
		draw_bar(pixelbuf, 0, (symbol->width + xoffset + xoffset), textoffset, symbol->border_width, image_width, image_height);
		draw_bar(pixelbuf, 0, (symbol->width + xoffset + xoffset), (textoffset + symbol->height + symbol->border_width), symbol->border_width, image_width, image_height);
		if(symbol->rows > 1) {
			/* row binding */
			for(r = 1; r < symbol->rows; r++) {
				draw_bar(pixelbuf, xoffset, symbol->width, ((r * row_height) + textoffset + yoffset - 1), 2, image_width, image_height);
			}
		}
	}
	
	if (symbol->output_options == BARCODE_BOX) {
		/* side bars */
		draw_bar(pixelbuf, 0, symbol->border_width, textoffset, symbol->height + (2 * symbol->border_width), image_width, image_height);
		draw_bar(pixelbuf, (symbol->width + xoffset + xoffset - symbol->border_width), symbol->border_width, textoffset, symbol->height + (2 * symbol->border_width), image_width, image_height);
	}
	
	/* Put the human readable text at the bottom */
	if((textdone == 0) && (strlen(symbol->text) != 0)) {
		textpos = (image_width / 2);
		draw_string(pixelbuf, symbol->text, textpos, (image_height - 17), image_width, image_height);
	}
	
	png_to_file(symbol, image_height, image_width, pixelbuf);
}