#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "weather.h"


// 示例输入：
// +HTTPCGET:261,{"results":[{"location":{"id":"WTEMH46Z5N09","name":"Hefei","country":"CN","path":"Hefei,Hefei,Anhui,China","timezone":"Asia/Shanghai","timezone_offset":"+08:00"},"now":{"text":"Cloudy","code":"4","temperature":"36"},"last_update":"2024-08-26T16:20:13+08:00"}]}
//
// OK


bool weather_parse(const char *data, weather_t *weather)
{
    char *p = strstr(data, "\"text\":\"");
    if (p == NULL)
    {
        return false;
    }
    p += strlen("\"text\":\"");
    char *q = strchr(p, '\"');
    if (q == NULL)
    {
        return false;
    }
    int len = q - p;
    if (len >= sizeof(weather->weather))
    {
        len = sizeof(weather->weather) - 1;
    }
    strncpy(weather->weather, p, len);
    weather->weather[len] = '\0';

    p = strstr(data, "\"temperature\":\"");
    if (p == NULL)
    {
        return false;
    }
    p += strlen("\"temperature\":\"");
    q = strchr(p, '\"');
    if (q == NULL)
    {
        return false;
    }
    len = q - p;
    if (len >= sizeof(weather->temperature))
    {
        len = sizeof(weather->temperature) - 1;
    }
    strncpy(weather->temperature, p, len);
    weather->temperature[len] = '\0';

    return true;
}
