#include "./text.h"
#include "./font.h"

unsigned int get_char_width(char charactor, struct MTK_WinFontMap *fontmap) {
	if (fontmap == 0 || (unsigned char)charactor < MTK_FONT_ASCII_CHAR_MIN || (unsigned char)charactor > MTK_FONT_ASCII_CHAR_MAX) {
		return 0;
	}
	return (unsigned int)(fontmap->offmaps[(unsigned char)charactor].width);
}
unsigned int get_text_width(char *string, struct MTK_WinFontMap *fontmap) {
	unsigned int width = 0;
	unsigned int i;
	if (fontmap == 0 || string == 0) {
		return 0;
	}
	i = 0;
	while (string[i] != 0) {
		width += get_char_width(string[i], fontmap);
		i++;
	}
	return width;
}

unsigned int draw_text(unsigned int x, unsigned int y, unsigned int max_width, unsigned int max_height, char *string, uint32_t color, struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window) {
	unsigned int i;
	unsigned int j;
	signed int width_check;
	
	if (fontmap == 0 || string == 0) {
		return 0;
	}
	
	i = 0;
	j = 0;
	while (string[i] != 0 && j < max_width) {
		width_check = max_width - j;
		if (width_check < 0) {
			width_check = 0;
		}
		j += draw_char(x + j, y, width_check, max_height, string[i], color, fontmap, window);
		i++;
	}
	
	return j;
}

unsigned int draw_char(unsigned int x, unsigned int y, unsigned int max_width, unsigned int max_height, char charactor, uint32_t color, struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window) {
	unsigned int i;
	unsigned int j;
	unsigned int alpha;
	
	if (fontmap == 0) {
		return 0;
	}
	if (charactor == 0) {
		return 0;
	}
	
	uint32_t working_alpha;
	alpha = 0xFF - (0xFF & (color >> 24));
	color &= 0x00FFFFFF;
	
	j = 0;
	while (j < fontmap->offmaps[(unsigned char)charactor].height && j < max_height) {
		i = 0;
		while (i < fontmap->offmaps[(unsigned char)charactor].width && i < max_width) {
			working_alpha = 0xFF & fontmap->offmaps[(unsigned char)charactor].char_bitmap[j * fontmap->offmaps[(unsigned char)charactor].width + i];
			working_alpha = (working_alpha * alpha) / 0xFF;
			working_alpha = 0xFF - working_alpha;
			fill_rect(x + i, y + j, 1, 1, color | (working_alpha << 24), window);
			i++;
		}
		j++;
	}
	
	return fontmap->offmaps[(unsigned char)charactor].width;
}
