#include "./font.h"
#include <ft2build.h>
#include FT_FREETYPE_H

void setup_font(char *font_file, unsigned int size, struct MTK_WinFontPack *font_pack){
	if(font_pack == 0 || font_file == 0 || size == 0) {
		return;
	}
	
	// Declare some usful variables
	unsigned int font_style_index;
	unsigned int charactor_index;
	unsigned int width;
	unsigned int height;
	unsigned int x;
	unsigned int y;
	unsigned int i;
	unsigned int max_map_width;
	unsigned int max_map_height;
	unsigned int max_pack_width;
	unsigned int max_pack_height;
	
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
		charactor_index = MTK_FONT_ASCII_CHAR_MIN;
		max_map_width = 0;
		max_map_height = 0;
		while (charactor_index <= MTK_FONT_ASCII_CHAR_MAX) {
			// Render the charactor in the library
			glyph_index = FT_Get_Char_Index(face, (unsigned char)(charactor_index & 0xFF));
			FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
			FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
			
			// Set some basic setting in the struct
			width = face->glyph->bitmap.width;
			height = face->glyph->bitmap.rows;
			font_pack->font_style[font_style_index].offmaps[charactor_index].width = width;
			font_pack->font_style[font_style_index].offmaps[charactor_index].height = height;
			font_pack->font_style[font_style_index].font_style = font_style_index;
			font_pack->font_style[font_style_index].font_size = size;
			if(width > max_map_width) {
				max_map_width = width;
			}
			if(height > max_map_height) {
				max_map_height = height;
			}
			
			// Allocate the current charactor's bitmap
			font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap = calloc(width, height);
			
			// Copy the bitmap
			for (x = 0; x < width; x++) {
				for (y = 0; y < height; y++) {
					i = width * y + x;
					font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap[i] = face->glyph->bitmap.buffer[i];
				}
			}
			
			charactor_index++;
		}
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
