#include <linux-i2c.h>
#include <u8g2.h>
#include <stdio.h>
#include <qrencode.h>

#define SSD1306_ADDR  0x3c

u8g2_t u8g2;

/*
 * module 4: quite small, LOW ECC -> max about 154 chars TBC
 * module 2 or 1: ok on OLED, LEW ECC -> max about 255 chars TBC
 * module 0: not recommended
 */
static void u8g2_draw_qr_lib(u8g2_t *u8, const char *text,
                             int x0, int y0, int max_w, int max_h, int border_modules) {

    QRcode *q = QRcode_encodeString(text, 0, QR_ECLEVEL_L, QR_MODE_8, 1);

    if (!q) {
      u8g2_DrawStr(u8, 2, 2, "Err QR");
      return;
    }

    int size = q->width;
    int scale = (max_w / (size + 2*border_modules));
    int scale_h = (max_h / (size + 2*border_modules));

    if (scale_h < scale)
      scale = scale_h;

    if (scale < 1) {
      u8g2_DrawStr(u8, 2, 2, "Err QR Scale");
      QRcode_free(q);
      return;
    }

    int qr_w = scale * (size + 2*border_modules);
    int x = x0 + (max_w - qr_w)/2;
    int y = y0 + (max_h - qr_w)/2;

    u8g2_SetDrawColor(u8, 0);
    u8g2_DrawBox(u8, x, y, qr_w, qr_w);
    u8g2_SetDrawColor(u8, 1);

    const unsigned char *p = q->data;
    for (int yy = 0; yy < size; yy++) {
        for (int xx = 0; xx < size; xx++, p++) {
            if (*p & 0x01) {
                int px = x + (border_modules + xx) * scale;
                int py = y + (border_modules + yy) * scale;
                u8g2_DrawBox(u8, px, py, scale, scale);
            }
        }
    }
    QRcode_free(q);
}

int main (void)
{
        u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
        //u8g2_Setup_sh1106_i2c_128x64_winstar_f(&u8g2, U8G2_R0, u8x8_byte_linux_i2c, u8x8_linux_i2c_delay);
	
	u8g2_SetI2CAddress(&u8g2, SSD1306_ADDR);

	u8g2_InitDisplay(&u8g2);

	u8g2_SetPowerSave(&u8g2, 0);

	u8g2_ClearBuffer(&u8g2);

	u8g2_SetFont(&u8g2, u8g2_font_smart_patrol_nbp_tr);

	u8g2_SetFontRefHeightText(&u8g2);

	u8g2_SetFontPosTop(&u8g2);

	u8g2_DrawStr(&u8g2, 4, 4, "XXX Demo XXX");

	u8g2_SendBuffer(&u8g2);

	sleep(5);

	u8g2_ClearBuffer(&u8g2);

	u8g2_draw_qr_lib(&u8g2,
"https://signup.free.fr/subscribe_promo/00_choose_offre.pl?pre_box=v9#v9",
	  0, 0, u8g2_GetDisplayWidth(&u8g2), u8g2_GetDisplayHeight(&u8g2), 1);

	u8g2_SendBuffer(&u8g2);
}
