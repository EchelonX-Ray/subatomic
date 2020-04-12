#include "./font.h"
#include <ft2build.h>
#include FT_FREETYPE_H

struct font_bitmaps {
	unsigned int width;
	unsigned int height;
	signed int left;
	signed int top;
	unsigned char *pixel_val;
};

void setup_font(char *font_file, unsigned int size, struct MTK_WinFontPack *font_pack){
	if(font_pack == 0 || font_file == 0 || size == 0) {
		return;
	}
	
	// Declare some usful variables
	unsigned int font_style_index;
	unsigned int charactor_index;
	signed int width;
	signed int height;
	signed int top;
	signed int left;
	signed int max_left;
	signed int min_left;
	signed int max_top;
	signed int min_top;
	unsigned int x;
	unsigned int y;
	unsigned int i;
	unsigned int j;
	unsigned int max_map_width;
	unsigned int max_map_height;
	unsigned int max_pack_width;
	unsigned int max_pack_height;
	struct font_bitmaps *temp_font_bitmaps;
	
	// Setup the font library
	FT_Library library;
	FT_Face face;
	FT_UInt glyph_index;
	FT_Init_FreeType(&library);
	FT_New_Face(library, font_file, 0, &face); // Load the font file
	FT_Set_Pixel_Sizes(face, 0, size); // Set the font size
	
	// Allocate the font style array
	font_pack->font_style = calloc(sizeof(struct MTK_WinFontMap), MTK_FONT_STYLE_COUNT);
	
	// Loop through the various font styles and Render them
	font_style_index = 0;
	max_pack_width = 0;
	max_pack_height = 0;
	while (font_style_index < MTK_FONT_STYLE_COUNT) {
		// Allocate the array of charator maps for the current font style
		font_pack->font_style[font_style_index].bitmaps = calloc(sizeof(struct MTK_WinFontCharactorBM), (MTK_FONT_ASCII_CHAR_MAX - MTK_FONT_ASCII_CHAR_MIN) + 1);
		font_pack->font_style[font_style_index].offmaps = font_pack->font_style[font_style_index].bitmaps - MTK_FONT_ASCII_CHAR_MIN;
		
		// Loop through and Render the printable charactors
		temp_font_bitmaps = calloc(sizeof(struct font_bitmaps), (MTK_FONT_ASCII_CHAR_MAX - MTK_FONT_ASCII_CHAR_MIN) + 1);
		j = 0;
		charactor_index = MTK_FONT_ASCII_CHAR_MIN;
		max_map_width = 0;
		max_map_height = 0;
		max_left = -10000;
		min_left = 10000;
		max_top = -10000;
		min_top = 10000;
		while (charactor_index <= MTK_FONT_ASCII_CHAR_MAX) {
			// Render the charactor in the library
			glyph_index = FT_Get_Char_Index(face, (unsigned char)(charactor_index & 0xFF));
			FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			
			// Set some basic setting in the struct
			width = face->glyph->bitmap.width;
			height = face->glyph->bitmap.rows;
			left = face->glyph->bitmap_left;
			top = face->glyph->bitmap_top;
			//font_pack->font_style[font_style_index].offmaps[charactor_index].width = width;
			//font_pack->font_style[font_style_index].offmaps[charactor_index].height = height;
			font_pack->font_style[font_style_index].font_style = font_style_index;
			font_pack->font_style[font_style_index].font_size = size;
			if (width + left > max_left) {
				max_left = width + left;
			}
			if (left < min_left) {
				min_left = left;
			}
			if (top > max_top) {
				max_top = top;
			}
			if (top - height < min_top) {
				min_top = top - height;
			}
			if (width + left > max_map_width) {
				max_map_width = width;
			}
			if (height + top > max_map_height) {
				max_map_height = height;
			}
			
			temp_font_bitmaps[j].width = width;
			temp_font_bitmaps[j].height = height;
			temp_font_bitmaps[j].left = left;
			temp_font_bitmaps[j].top = top;
			temp_font_bitmaps[j].pixel_val = calloc(width, height);
			
			// Copy the bitmap
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					i = width * y + x;
					//j = width * (y + top) + (x + left);
					//font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap[j] = face->glyph->bitmap.buffer[i];
					//font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap[i] = face->glyph->bitmap.buffer[i];
					temp_font_bitmaps[j].pixel_val[i] = face->glyph->bitmap.buffer[i];
				}
			}
			
			j++;
			charactor_index++;
		}
		max_map_width = max_left - min_left;
		max_map_height = max_top - min_top;
		
		j = 0;
		charactor_index = MTK_FONT_ASCII_CHAR_MIN;
		while(j < (MTK_FONT_ASCII_CHAR_MAX - MTK_FONT_ASCII_CHAR_MIN) + 1) {
			font_pack->font_style[font_style_index].offmaps[charactor_index].width = max_map_width;
			font_pack->font_style[font_style_index].offmaps[charactor_index].height = max_map_height;
			font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap = calloc(max_map_width, max_map_height);
			width = temp_font_bitmaps[j].width;
			height = temp_font_bitmaps[j].height;
			left = temp_font_bitmaps[j].left;
			top = temp_font_bitmaps[j].top;
			
			// Copy the bitmap
			for (y = 0; y < height; y++) {
				for (x = 0; x < width; x++) {
					i = max_map_width * (y + (max_top - top)) + (x + left - min_left);
					font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap[i] = temp_font_bitmaps[j].pixel_val[width * y + x];
				}
			}
			
			j++;
			charactor_index++;
		}
		j = 0;
		while(j < (MTK_FONT_ASCII_CHAR_MAX - MTK_FONT_ASCII_CHAR_MIN) + 1) {
			free(temp_font_bitmaps[j].pixel_val);
			j++;
		}
		free(temp_font_bitmaps);
		
		font_pack->font_style[font_style_index].max_width = max_map_width;
		font_pack->font_style[font_style_index].max_height = max_map_height;
		if(max_map_width > max_pack_width) {
			max_pack_width = max_map_width;
		}
		if(max_map_height > max_pack_height) {
			max_pack_height = max_map_height;
		}
		
		font_style_index++;
	}
	font_pack->max_width = max_pack_width;
	font_pack->max_height = max_pack_height;
	
	// Free up library allocations
	// These should be in the reverse order that they were created
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	return;
}

void free_font(struct MTK_WinFontPack *font_pack){
	if(font_pack == 0) {
		return;
	}
	
	// Declare some usful variables
	unsigned int font_style_index;
	unsigned int charactor_index;
	
	// Loop through the various font styles
	font_style_index = 0;
	while (font_style_index < MTK_FONT_STYLE_COUNT) {
		// Loop through the printable charactors
		charactor_index = MTK_FONT_ASCII_CHAR_MIN;
		while (charactor_index <= MTK_FONT_ASCII_CHAR_MAX) {
			// Free the current charactor's bitmap
			free(font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap);
			
			charactor_index++;
		}
		free(font_pack->font_style[font_style_index].bitmaps);
		
		font_style_index++;
	}
	free(font_pack->font_style);
	font_pack->font_style = 0;
	return;
}
