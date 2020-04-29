#include "./font.h"
#include "./../toolbox/cstr_manip.h"
#include <ft2build.h>
#include <fontconfig/fontconfig.h>
#include <stdlib.h>
#include FT_FREETYPE_H

struct font_bitmaps {
	unsigned int width;
	unsigned int height;
	signed int left;
	signed int top;
	unsigned char *pixel_val;
};

void load_font_family(char *family, struct MTK_WinFontPack *font_pack) {
	font_pack->files = calloc(sizeof(char*), MTK_FONT_STYLE_COUNT);
	
	FcConfig *config = FcInitLoadConfigAndFonts();
	FcPattern* pat = FcPatternCreate();
	FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *)0);
	FcFontSet* fs = 0;
	if (config && pat && os) {
		fs = FcFontList(config, pat, os);
	}
	printf("Total matching fonts: %d\n", fs->nfont);
	if (fs) {
		for (int i = 0; fs && i < fs->nfont; i++) {
			FcPattern* font = fs->fonts[i];
			FcChar8 *ffile, *fstyle, *ffamily;
			ffile = 0;
			fstyle = 0;
			ffamily = 0;
			if (	FcPatternGetString(font, FC_FILE, 0, &ffile) == FcResultMatch &&
					FcPatternGetString(font, FC_FAMILY, 0, &ffamily) == FcResultMatch &&
					FcPatternGetString(font, FC_STYLE, 0, &fstyle) == FcResultMatch	) {
				if (cstrcmp(family, (char*)ffamily)) {
					printf("Trace_Font Style: %s\n", fstyle);
					unsigned int i;
					i = 0;
					while (i < MTK_FONT_STYLE_COUNT) {
						if (i == (MTK_FONT_STYLE_NORMAL)) {
							if (cstrcmp("Book", (char*)fstyle)) {
								font_pack->files[i] = calloc(sizeof(char), cstrlen((char*)ffile));
								cstrcpy((char*)ffile, font_pack->files[i]);
							}
						}
						if (i == (MTK_FONT_STYLE_BOLD)) {
							if (cstrcmp("Bold", (char*)fstyle)) {
								font_pack->files[i] = calloc(sizeof(char), cstrlen((char*)ffile));
								cstrcpy((char*)ffile, font_pack->files[i]);
							}
						}
						if (i == (MTK_FONT_STYLE_ITALIC)) {
							if (cstrcmp("Regular Italic", (char*)fstyle)) {
								font_pack->files[i] = calloc(sizeof(char), cstrlen((char*)ffile));
								cstrcpy((char*)ffile, font_pack->files[i]);
							}
						}
						if (i == (MTK_FONT_STYLE_BOLD | MTK_FONT_STYLE_ITALIC)) {
							if (cstrcmp("Bold Italic", (char*)fstyle)) {
								font_pack->files[i] = calloc(sizeof(char), cstrlen((char*)ffile));
								cstrcpy((char*)ffile, font_pack->files[i]);
							}
						}
						i++;
					}
				}
			}
		}
		FcFontSetDestroy(fs);
	}
	if (os) {
		FcObjectSetDestroy(os);
	}
	if (pat) {
		FcPatternDestroy(pat);
	}
	if (config) {
		FcConfigDestroy(config);
	}
	
	return;
}

void setup_font(unsigned int size, struct MTK_WinFontPack *font_pack) {
	if(font_pack == 0 || size == 0) {
		return;
	}
	
	// Declare some useful variables
	unsigned int font_style_index;
	unsigned int charactor_index;
	signed int adv_width;
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
	
	// Allocate the font style array
	font_pack->font_style = calloc(sizeof(struct MTK_WinFontMap), MTK_FONT_STYLE_COUNT);
	
	// Loop through the various font styles and Render them
	font_style_index = 0;
	max_pack_width = 0;
	max_pack_height = 0;
	while (font_style_index < MTK_FONT_STYLE_COUNT) {
		if (font_pack->files[font_style_index]) {
			FT_New_Face(library, font_pack->files[font_style_index], 0, &face); // Load the font file
			FT_Set_Pixel_Sizes(face, 0, size); // Set the font size
			
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
				adv_width = face->glyph->metrics.horiAdvance / 64;
				width = face->glyph->metrics.width / 64;
				height = face->glyph->metrics.height / 64;
				left = face->glyph->metrics.horiBearingX / 64;
				top = face->glyph->metrics.horiBearingY / 64;
				font_pack->font_style[font_style_index].offmaps[charactor_index].width = width;
				font_pack->font_style[font_style_index].offmaps[charactor_index].height = height;
				font_pack->font_style[font_style_index].offmaps[charactor_index].adv_width = adv_width;
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
			while (j < (MTK_FONT_ASCII_CHAR_MAX - MTK_FONT_ASCII_CHAR_MIN) + 1) {
				width = temp_font_bitmaps[j].width;
				height = temp_font_bitmaps[j].height;
				left = temp_font_bitmaps[j].left;
				top = max_top - temp_font_bitmaps[j].top;
				font_pack->font_style[font_style_index].offmaps[charactor_index].max_width_pad_left = left;
				font_pack->font_style[font_style_index].offmaps[charactor_index].max_width_pad_right = max_map_width - (width + left);
				font_pack->font_style[font_style_index].offmaps[charactor_index].max_height_pad_top = top;
				font_pack->font_style[font_style_index].offmaps[charactor_index].max_height_pad_bottom = max_map_height - (height + top);
				font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap = calloc(width, height);
				
				if (left < 0) {
					font_pack->font_style[font_style_index].offmaps[charactor_index].max_width_pad_left = 0;
					font_pack->font_style[font_style_index].offmaps[charactor_index].adv_width -= left;
				}
				
				// Copy the bitmap
				for (y = 0; y < height; y++) {
					for (x = 0; x < width; x++) {
						i = width * y + x;
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
			
			// Free up the font face
			FT_Done_Face(face);
			free(font_pack->files[font_style_index]);
		}
		font_style_index++;
	}
	font_pack->max_width = max_pack_width;
	font_pack->max_height = max_pack_height;
	
	// Free up the library allocation
	FT_Done_FreeType(library);
	// Free up the list of files
	free(font_pack->files);
	
	return;
}

void free_font(struct MTK_WinFontPack *font_pack) {
	if(font_pack == 0) {
		return;
	}
	
	// Declare some usful variables
	unsigned int font_style_index;
	unsigned int charactor_index;
	
	// Loop through the various font styles
	font_style_index = 0;
	if (font_pack->font_style) {
		while (font_style_index < MTK_FONT_STYLE_COUNT) {
			// Loop through the printable charactors
			charactor_index = MTK_FONT_ASCII_CHAR_MIN;
			if (font_pack->font_style[font_style_index].offmaps) {
				while (charactor_index <= MTK_FONT_ASCII_CHAR_MAX) {
					// Free the current charactor's bitmap
					
					if (font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap) {
						free(font_pack->font_style[font_style_index].offmaps[charactor_index].char_bitmap);
					}
					
					charactor_index++;
				}
			}
			if (font_pack->font_style[font_style_index].bitmaps) {
				free(font_pack->font_style[font_style_index].bitmaps);
			}
			
			font_style_index++;
		}
		free(font_pack->font_style);
	}
	font_pack->font_style = 0;
	return;
}
