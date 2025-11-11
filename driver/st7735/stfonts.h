#ifndef __ST_FONTS_H__
#define __ST_FONTS_H__


#include <stdint.h>


typedef struct
{
    const uint16_t width;
    const uint16_t height;
    const uint8_t *data;
    const uint32_t count;
} st_fonts_t;


extern st_fonts_t font_ascii_8x16;
extern st_fonts_t font_temper_16x32;
extern st_fonts_t font_location_32x32;
extern st_fonts_t font_du_32x32;
extern st_fonts_t font_character_16x16;
extern st_fonts_t font_du_16x16;
extern st_fonts_t font_time_24x48;
extern st_fonts_t font_lucky_24x48;


#endif /* __ST_FONTS_H__ */
