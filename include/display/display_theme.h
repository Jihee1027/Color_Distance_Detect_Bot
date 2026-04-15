#ifndef DISPLAY_THEME_H
#define DISPLAY_THEME_H

#define DISP_SPI_PORT   spi0
#define DISP_PIN_SCK    18
#define DISP_PIN_TX     19
#define DISP_PIN_CSN    17
#define DISP_PIN_DC     12
#define DISP_PIN_RST    11

#define TFT_WIDTH   320
#define TFT_HEIGHT  240

#define COLOR_BLACK         0x0000
#define COLOR_WHITE         0xFFFF
#define COLOR_RED 0xF800

#define COLOR_SKY_TOP       0x2C7F
#define COLOR_SKY_MID1      0x4D9F
#define COLOR_SKY_MID2      0x6E9F
#define COLOR_SKY_LOW       0xA77F

#define COLOR_CLOUD         0xFFFF
#define COLOR_CLOUD_SHADE   0xE71C
#define COLOR_CLOUD_DARK    0xBDF7

#define COLOR_GRASS         0x7FE0
#define COLOR_GRASS_DARK    0x3666
#define COLOR_HILL          0x5F40
#define COLOR_HILL_DARK     0x3DE0

#define COLOR_DIRT          0xDCC6
#define COLOR_DIRT_DARK     0xAB24
#define COLOR_DIRT_LIGHT    0xEE08

#define COLOR_PANEL_FILL    0xFC9F

#define COLOR_START_MAIN    0xFD20
#define COLOR_START_LIGHT   0xFFE0
#define COLOR_START_SHADOW  0xA145
#define COLOR_START_OUTLINE 0x780F

#define COLOR_TEXT_BOX      0xEEFF

#define COLOR_WHITE_SOFT      0xF7DE
#define COLOR_PINK_LIGHT      0xFE9F
#define COLOR_PINK_DARK       0xC218
#define COLOR_BOX_BLUE_LIGHT  0xFFFF
#define COLOR_BOX_BLUE_DARK   0x2C12
#define COLOR_SHADOW_STRONG   0x0841

#define BTN_BUF_W      228
#define BTN_BUF_H       42
#define BTN_SCREEN_X    46
#define BTN_SCREEN_Y   170

#define BTN_BASE_W     210
#define BTN_BASE_H      34
#define BTN_TEXT_SCALE   2

#define START_BUF_W 210
#define START_BUF_H 88
#define START_SCREEN_X 57
#define START_SCREEN_Y 60
#define START_BASE_W 206
#define START_BASE_H 84

#define COLOR_TITLE_BUF_W      224
#define COLOR_TITLE_BUF_H       38
#define COLOR_TITLE_SCREEN_X    48
#define COLOR_TITLE_SCREEN_Y    30

#define COLOR_TITLE_BASE_W     220
#define COLOR_TITLE_BASE_H      34
#define COLOR_TITLE_TEXT_SCALE   2

#define COLOR_OPTION_BUF_W       166
#define COLOR_OPTION_BUF_H        46
#define COLOR_OPTION_SCREEN_X     77
#define COLOR_OPTION_SCREEN_Y     96
#define COLOR_OPTION_BASE_W      160
#define COLOR_OPTION_BASE_H       42
#define COLOR_OPTION_TEXT_SCALE    3

#define COLOR_HINT_BUF_W         320
#define COLOR_HINT_BUF_H          24
#define COLOR_HINT_SCREEN_X        0
#define COLOR_HINT_SCREEN_Y      214
#define COLOR_HINT_TEXT_SCALE      2

#define COLOR_WINDOW_BLUE       0x4DDF
#define COLOR_WINDOW_HIGHLIGHT  0xBFFF

#define COLOR_HEADLIGHT         0xFFE0
#define COLOR_TAILLIGHT         0xFBE0
#define COLOR_HANDLE            0x2945

#define COLOR_WHEEL_OUTER       0x2104
#define COLOR_WHEEL_INNER       0xDEFB
#define COLOR_WHEEL_HIGHLIGHT   0xFFFF
#define COLOR_GROUND_SHADOW     0x4208

#define COLOR_CAR_RED_DARK      0xA800
#define COLOR_CAR_RED_LIGHT     0xFB04

#define COLOR_BLUE_DARK         0x0010
#define COLOR_BLUE_LIGHT        0x7D7F

#define COLOR_GREEN_DARK        0x03E0
#define COLOR_GREEN_LIGHT       0x87F0

#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF

#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F

#define COLOR_YELLOW  0xFFE0
#define COLOR_ORANGE  0xFD20
#define COLOR_CYAN    0x07FF
#define COLOR_MAGENTA 0xF81F

#endif