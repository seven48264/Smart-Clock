#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32f10x.h"
#include "main.h"
#include "lcd_spi.h"
#include "st7735.h"
#include "stfonts.h"



// 取模配置：阴码 + 顺向(横向逐行) + 高位在前
#define GLYPH_LSB_FIRST  0   // 0=高位在前(MSB first)
#define GLYPH_YANG_CODE  0   // 0=阴码(0=亮,1=灭)

#define CS_PORT     GPIOA
#define CS_PIN      GPIO_Pin_4
#define DC_PORT     GPIOB
#define DC_PIN      GPIO_Pin_1
#define RES_PORT    GPIOB
#define RES_PIN     GPIO_Pin_0
// #define BLK_PORT    GPIOA
// #define BLK_PIN     0

#define GRAM_BUFFER_SIZE 4096

// ST7735 Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_GAMSET  0x26
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   0x30
#define ST7735_COLMOD  0x3A
#define ST7735_MADCTL  0x36

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define CMD_delay_ms      0xFF
#define CMD_EOF        0xFF


static const uint8_t init_cmd_list[] =
{
    // 0x11,  0,
    // CMD_delay_ms, 12,
    // 0xB1,  3,  0x05, 0x3C, 0x3C,
    // 0xB2,  3,  0x05, 0x3C, 0x3C,
    // 0xB3,  6,  0x05, 0x3C, 0x3C, 0x05, 0x3C, 0x3C,
    // 0xB4,  1,  0x03,
    // 0xC0,  3,  0x28, 0x08, 0x04,
    // 0xC1,  1,  0XC0,
    // 0xC2,  2,  0x0D, 0x00,
    // 0xC3,  2,  0x8D, 0x2A,
    // 0xC4,  2,  0x8D, 0xEE,
    // 0xC5,  1,  0x10,
    // 0xE0,  16, 0x04, 0x22, 0x07, 0x0A, 0x2E, 0x30, 0x25, 0x2A, 0x28, 0x26, 0x2E, 0x3A, 0x00, 0x01, 0x03, 0x13,
    // 0xE1,  16, 0x04, 0x16, 0x06, 0x0D, 0x2D, 0x26, 0x23, 0x27, 0x27, 0x25, 0x2D, 0x3B, 0x00, 0x01, 0x04, 0x13,
    // CMD_delay_ms, CMD_EOF,

    0x11,  0,
    CMD_delay_ms, 12,
    0xB1,  3,  0x01, 0x2C, 0x2D,
    0xB2,  3,  0x01, 0x2C, 0x2D,
    0xB3,  6,  0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    0xB4,  1,  0x07,
    0xC0,  3,  0xA2, 0x02, 0x84,
    0xC1,  1,  0xC5,
    0xC2,  2,  0x0A, 0x00,
    0xC3,  2,  0x8A, 0x2A,
    0xC4,  2,  0x8A, 0xEE,
    0xC5,  1,  0x0E,
    0x36,  1,  0xC8,
    0xE0,  16, 0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,
    0xE1,  16, 0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10,
    0x2A,  4,  0x00, 0x00, 0x00, 0x7F,
    0x2B,  4,  0x00, 0x00, 0x00, 0x9F,
    0xF6,  1,  0x00,
    0x3A,  1,  0x05,
    0x29,  0,

    CMD_delay_ms, CMD_EOF,
};

static volatile bool spi_async_done;
static uint8_t gram_buff[GRAM_BUFFER_SIZE];


static void spi_on_async_finish(void)
{
    spi_async_done = true;
}

static void st7735_select(void)
{
    GPIO_WriteBit(CS_PORT, CS_PIN, Bit_RESET);
}

void st7735_unselect(void)
{
    GPIO_WriteBit(CS_PORT, CS_PIN, Bit_SET);
}

static void st7735_reset(void)
{
    GPIO_WriteBit(RES_PORT, RES_PIN, Bit_RESET);
    delay_ms(2);
    GPIO_WriteBit(RES_PORT, RES_PIN, Bit_SET);
    delay_ms(150);
}

// static void st7735_bl_on(void)
// {
//     //GPIO_WriteBit(BLK_PORT, BLK_PIN, Bit_SET);
// }

// static void st7735_bl_off(void)
// {
//     //GPIO_WriteBit(BLK_PORT, BLK_PIN, Bit_RESET);
// }

static void st7735_write_cmd(uint8_t cmd)
{
    GPIO_WriteBit(DC_PORT, DC_PIN, Bit_RESET);
    lcd_spi_write(&cmd, 1);
}

static void st7735_write_data(uint8_t *data, size_t size)
{
    GPIO_WriteBit(DC_PORT, DC_PIN, Bit_SET);
    spi_async_done = false;
    lcd_spi_write_async(data, size);
    while (!spi_async_done);
}

static void st7735_exec_cmds(const uint8_t *cmd_list)
{
    while (1)
    {
        uint8_t cmd = *cmd_list++;
        uint8_t num = *cmd_list++;
        if (cmd == CMD_delay_ms)
        {
            if (num == CMD_EOF)
                break;
            else
                delay_ms(num * 10);
        }
        else
        {
            st7735_write_cmd(cmd);
            if (num > 0) {
                st7735_write_data((uint8_t *)cmd_list, num);
            }
            cmd_list += num;
        }
    }
}

static void st7735_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // 列地址设置
    st7735_write_cmd(ST7735_CASET);
    uint8_t data[] = {0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART};
    st7735_write_data(data, sizeof(data));

    // 行地址设置
    st7735_write_cmd(ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    st7735_write_data(data, sizeof(data));

    // 写入内存
    st7735_write_cmd(ST7735_RAMWR);
}

static void st7735_pin_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // CS
    GPIO_InitStructure.GPIO_Pin = CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(CS_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(CS_PORT, CS_PIN, Bit_SET);

    // DC
    GPIO_InitStructure.GPIO_Pin = DC_PIN;
    GPIO_Init(DC_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(DC_PORT, DC_PIN, Bit_RESET);

    // RES
    GPIO_InitStructure.GPIO_Pin = RES_PIN;
    GPIO_Init(RES_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(RES_PORT, RES_PIN, Bit_RESET);

    // // BLK
    // GPIO_InitStructure.GPIO_Pin = BLK_PIN;
    // GPIO_Init(BLK_PORT, &GPIO_InitStructure);
    // GPIO_WriteBit(BLK_PORT, BLK_PIN, Bit_RESET);
}

void st7735_init()
{
    lcd_spi_init();
    lcd_spi_send_finish_register(spi_on_async_finish);
    st7735_pin_init();

    st7735_reset();

    st7735_select();
    st7735_exec_cmds(init_cmd_list);
    st7735_unselect();

    //st7735_bl_on();
}

void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
        return;

    uint8_t data[] = {color >> 8, color & 0xFF};

    st7735_select();
    st7735_set_window(x, y, x + 1, y + 1);
    st7735_write_data(data, sizeof(data));
    st7735_unselect();
}

static const uint8_t *st7735_find_font(const st_fonts_t *font, char ch)
{
    uint32_t bytes_per_line = (font->width + 7) / 8;

    for (uint32_t i = 0; i < font->count; i++)
    {
        const uint8_t *pcode = font->data + i * (font->height * bytes_per_line + 1);
        if (*pcode == ch)
        {
            return pcode + 1;
        }
    }

    return NULL;
}

void st7735_write_char(uint16_t x, uint16_t y, char ch, st_fonts_t *font, uint16_t color, uint16_t bgcolor)
{
    st7735_select();

    st7735_set_window(x, y, x + font->width - 1, y + font->height - 1);

    uint32_t bytes_per_line = (font->width + 7) / 8;

    uint8_t *pbuff = gram_buff;
    const uint8_t *fcode = st7735_find_font(font, ch);
    for (uint32_t y = 0; y < font->height; y++)
    {
        const uint8_t *pcode = fcode + y * bytes_per_line;
        for (uint32_t x = 0; x < font->width; x++)
        {
            uint8_t b = pcode[x >> 3];
            if ((b << (x & 0x7)) & 0x80)
            {
                *pbuff++ = color >> 8;
                *pbuff++ = color & 0xFF;
            }
            else
            {
                *pbuff++ = bgcolor >> 8;
                *pbuff++ = bgcolor & 0xFF;
            }
        }
    }

    st7735_write_data(gram_buff, pbuff - gram_buff);

    st7735_unselect();
}

void st7735_write_string(uint16_t x, uint16_t y, const char *str, st_fonts_t *font, uint16_t color, uint16_t bgcolor)
{
    while (*str)
    {
        if (x + font->width >= ST7735_WIDTH)
        {
            x = 0;
            y += font->height;
            if (y + font->height >= ST7735_HEIGHT)
            {
                break;
            }

            if (*str == ' ')
            {
                str++;
                continue;
            }
        }

        st7735_write_char(x, y, *str, font, color, bgcolor);
        x += font->width;
        str++;
    }

}

// 直接按索引绘制位图（用于纯位图字库，如中文16x16，不带码头字节）
static void st7735_write_glyph_by_index(uint16_t x, uint16_t y,
                                         const st_fonts_t *font, uint32_t index,
                                         uint16_t color, uint16_t bgcolor)
{
    uint32_t bpl = (font->width + 7) / 8;              // bytes per line
    uint32_t glyph_size = bpl * font->height;          // 每字节数 = 行字节 * 行数
    const uint8_t *fcode = font->data + index * glyph_size;

    st7735_select();
    st7735_set_window(x, y, x + font->width - 1, y + font->height - 1);

    uint8_t *pbuff = gram_buff;

    for (uint32_t yy = 0; yy < font->height; yy++) {
        const uint8_t *row = fcode + yy * bpl;
        for (uint32_t xx = 0; xx < font->width; xx++) {
            uint8_t byte = row[xx >> 3];                    // 从字形数据按行读取字节
            uint8_t bit = (byte & (0x80 >> (xx & 7))) != 0; // 高位在前

            uint16_t px = bit ? color : bgcolor;           // 阳码：1亮；阴码：0亮
            *pbuff++ = px >> 8;
            *pbuff++ = px & 0xFF;
        }
    }

    st7735_write_data(gram_buff, pbuff - gram_buff);
    st7735_unselect();
}


void st7735_write_font(uint16_t x, uint16_t y, st_fonts_t *font, uint32_t index, uint16_t color, uint16_t bgcolor)
{
    st7735_write_glyph_by_index(x, y, font, index, color, bgcolor);
}

void st7735_write_fonts(uint16_t x, uint16_t y, st_fonts_t *font, uint32_t index, uint32_t count, uint16_t color, uint16_t bgcolor)
{
    uint32_t end = index + count;
    if (end > font->count) end = font->count;

    for (uint32_t i = index; i < end; i++) {
        if (x + font->width >= ST7735_WIDTH) {
            x = 0;
            y += font->height;
            if (y + font->height >= ST7735_HEIGHT) break;
        }
        st7735_write_glyph_by_index(x, y, font, i, color, bgcolor);
        x += font->width;
    }
}

void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
        return;
    if (x + w - 1 >= ST7735_WIDTH)
        w = ST7735_WIDTH - x;
    if (y + h - 1 >= ST7735_HEIGHT)
        h = ST7735_HEIGHT - y;

    st7735_select();
    st7735_set_window(x, y, x + w - 1, y + h - 1);

    uint8_t pixel[2] = {color >> 8, color & 0xFF};
    for (uint32_t i = 0; i < w * h; i += GRAM_BUFFER_SIZE / 2)
    {
        uint32_t size = w * h - i;
        if (size > GRAM_BUFFER_SIZE / 2)
            size = GRAM_BUFFER_SIZE / 2;
        if (i == 0)
        {
            uint8_t *pbuff = gram_buff;
            for (uint32_t j = 0; j < size; j++)
            {
                *pbuff++ = pixel[0];
                *pbuff++ = pixel[1];
            }
        }
        st7735_write_data(gram_buff, size * 2);
    }

    st7735_unselect();
}

void st7735_fill_screen(uint16_t color)
{
    st7735_fill_rect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void st7735_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data)
{
    if ((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT))
        return;
    if ((x + w - 1) >= ST7735_WIDTH)
        return;
    if ((y + h - 1) >= ST7735_HEIGHT)
        return;

    st7735_select();
    st7735_set_window(x, y, x + w - 1, y + h - 1);
    st7735_write_data((uint8_t *) data, w * h * 2);
    st7735_unselect();
}
