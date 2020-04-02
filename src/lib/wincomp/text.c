#include "./text.h"

unsigned int draw_char(unsigned int x, unsigned int y, unsigned char charactor, uint32_t color, struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window){
	unsigned int i;
	unsigned int j;
	unsigned int alpha;
	
	uint32_t working_alpha;
	alpha = 0xFF - (0xFF & (color >> 24));
	color &= 0x00FFFFFF;
	
	j = 0;
	while (j < fontmap->offmaps[charactor].height) {
		i = 0;
		while (i < fontmap->offmaps[charactor].width) {
			working_alpha = 0xFF & fontmap->offmaps[charactor].char_bitmap[j * fontmap->offmaps[charactor].width + i];
			working_alpha = (working_alpha * alpha) / 0xFF;
			working_alpha = 0xFF - working_alpha;
			fill_rect(x + i, y + j, 1, 1, color | (working_alpha << 24), window);
			i++;
		}
		j++;
	}
	
	return fontmap->offmaps[charactor].width;
}
