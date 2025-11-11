#ifndef __WEATHER_H__
#define __WEATHER_H__


typedef struct
{
    char weather[32];
    char temperature[8];
} weather_t;


bool weather_parse(const char *data, weather_t *weather);


#endif /* __WEATHER_H__ */
