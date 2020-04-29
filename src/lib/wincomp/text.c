#include "./text.h"
#include "./font.h"

unsigned int get_char_width(char charactor, struct MTK_WinFontMap *fontmap) {
	if (fontmap == 0 || (unsigned char)charactor < MTK_FONT_ASCII_CHAR_MIN || (unsigned char)charactor > MTK_FONT_ASCII_CHAR_MAX) {
		return 0;
	}
	if (fontmap->bitmaps == 0 || fontmap->offmaps == 0) {
		return 0;
	}
	return fontmap->offmaps[(unsigned char)charactor].adv_width;
}
unsigned int get_char_height(char charactor, struct MTK_WinFontMap *fontmap) {
	if (fontmap == 0 || (unsigned char)charactor < MTK_FONT_ASCII_CHAR_MIN || (unsigned char)charactor > MTK_FONT_ASCII_CHAR_MAX) {
		return 0;
	}
	if (fontmap->bitmaps == 0 || fontmap->offmaps == 0) {
		return 0;
	}
	return 	fontmap->offmaps[(unsigned char)charactor].max_height_pad_top + \
				fontmap->offmaps[(unsigned char)charactor].height + \
				fontmap->offmaps[(unsigned char)charactor].max_height_pad_bottom;
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

unsigned int draw_text(	unsigned int x, unsigned int y, unsigned int clip_width, unsigned int clip_height, 
								unsigned int max_width, unsigned int max_height, char *string, uint32_t color, 
								struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window) {
	unsigned int i;
	unsigned int j;
	unsigned int k;
	unsigned int char_width;
	signed int width_check;
	
	if (fontmap == 0 || string == 0) {
		return 0;
	}
	
	i = 0;
	j = 0;
	k = 0;
	while (string[i] != 0 && j < max_width) {
		width_check = max_width - j;
		if (width_check == 0) {
			break;
		}
		char_width = get_char_width(string[i], fontmap);
		k += char_width;
		if (k >= clip_width) {
			if (k - clip_width < char_width) {
				j += draw_char(x + j, y, char_width - (k - clip_width), clip_height, width_check, max_height, string[i], color, fontmap, window);
			} else {
				j += draw_char(x + j, y, 0, clip_height, width_check, max_height, string[i], color, fontmap, window);
			}
		}
		i++;
	}
	
	return j;
}

unsigned int draw_char(	unsigned int x, unsigned int y, unsigned int clip_width, unsigned int clip_height, 
								unsigned int max_width, unsigned int max_height, char charactor, uint32_t color, 
								struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window) {
	unsigned int a;
	unsigned int b;
	unsigned int i;
	unsigned int j;
	unsigned int max_height_pad_top;
	unsigned int max_width_pad_left;
	unsigned int alpha;
	
	if (fontmap == 0) {
		return 0;
	}
	if (fontmap->bitmaps == 0 || fontmap->offmaps == 0) {
		return 0;
	}
	if (fontmap->offmaps[(unsigned char)charactor].char_bitmap == 0) {
		return 0;
	}
	if (charactor == 0) {
		return 0;
	}
	
	uint32_t working_alpha;
	alpha = 0xFF - (0xFF & (color >> 24));
	color &= 0x00FFFFFF;
	
	//max_width_pad_left = 0;
	max_width_pad_left = fontmap->offmaps[(unsigned char)charactor].max_width_pad_left;
	//max_height_pad_top = 0;
	max_height_pad_top = fontmap->offmaps[(unsigned char)charactor].max_height_pad_top;
	
	b = max_height_pad_top;
	j = clip_height;
	while (j < fontmap->offmaps[(unsigned char)charactor].height && b < max_height) {
		a = max_width_pad_left;
		i = clip_width;
		while (i < fontmap->offmaps[(unsigned char)charactor].width && a < max_width) {
			working_alpha = 0xFF & fontmap->offmaps[(unsigned char)charactor].char_bitmap[j * fontmap->offmaps[(unsigned char)charactor].width + i];
			working_alpha = (working_alpha * alpha) / 0xFF;
			working_alpha = 0xFF - working_alpha;
			fill_rect(x + a, y + b, 1, 1, color | (working_alpha << 24), window);
			a++;
			i++;
		}
		b++;
		j++;
	}
	
	i = fontmap->offmaps[(unsigned char)charactor].adv_width;
	if (i > max_width) {
		i = max_width;
	}
	if (clip_width >= i) {
		return 0;
	} else {
		return i - clip_width;
	}
}
