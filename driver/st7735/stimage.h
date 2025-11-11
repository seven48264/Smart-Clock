#ifndef __ST_IMAGE_H__
#define __ST_IMAGE_H__


#include <stdint.h>


typedef struct
{
    const uint16_t width;
    const uint16_t height;
    const uint8_t *data;
} st_image_t;


extern const st_image_t icon_weather_duopyun;
extern const st_image_t icon_weather_feng;
extern const st_image_t icon_weather_qing;
extern const st_image_t icon_weather_xue;
extern const st_image_t icon_weather_yin;
extern const st_image_t icon_weather_yu;
extern const st_image_t icon_location;


#endif /* __ST_IMAGE_H__ */
