// ═══════════════════════════════════════════════════════════════════════════
// HaleHound-CYD User_Setup.h
// TFT_eSPI Configuration for Cheap Yellow Display Boards
// Created: 2026-02-06
// ═══════════════════════════════════════════════════════════════════════════
//
// BOARD SELECTION: Uncomment ONE of the following lines
// ═══════════════════════════════════════════════════════════════════════════

#define CYD_28    // ESP32-2432S028 - 2.8" 320x240 ILI9341
//#define CYD_35    // ESP32-3248S035 - 3.5" 480x320 ST7796

// ═══════════════════════════════════════════════════════════════════════════
// USER DEFINED SETTINGS - TFT_eSPI Library Configuration
// ═══════════════════════════════════════════════════════════════════════════

#define USER_SETUP_INFO "HaleHound-CYD"

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 1: DISPLAY DRIVER SELECTION
// ═══════════════════════════════════════════════════════════════════════════

#ifdef CYD_28
  // 2.8" CYD uses ILI9341
  #define ILI9341_DRIVER
  #define TFT_WIDTH  240
  #define TFT_HEIGHT 320
  #define CYD_BOARD_NAME "CYD 2.8\" (ESP32-2432S028)"
#endif

#ifdef CYD_35
  // 3.5" CYD uses ST7796
  #define ST7796_DRIVER
  #define TFT_WIDTH  320
  #define TFT_HEIGHT 480
  #define CYD_BOARD_NAME "CYD 3.5\" (ESP32-3248S035)"
#endif

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 2: DISPLAY SPI PINS (HSPI)
// Same for both boards
// ═══════════════════════════════════════════════════════════════════════════

#define TFT_MISO 12
#define TFT_MOSI 13
#define TFT_SCLK 14
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST  -1   // Connected to EN/RST on both boards

// Backlight pin differs between boards
#ifdef CYD_28
  #define TFT_BL   21   // 2.8" backlight on GPIO21
#endif
#ifdef CYD_35
  #define TFT_BL   27   // 3.5" backlight on GPIO27
#endif

#define TFT_BACKLIGHT_ON HIGH

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 2B: TOUCH CONTROLLER PINS
// ═══════════════════════════════════════════════════════════════════════════

// Touch chip select - same for both boards
#define TOUCH_CS 33

#ifdef CYD_28
  // 2.8" has DEDICATED touch SPI bus (separate from display)
  #define CYD_TOUCH_IRQ   36
  #define CYD_TOUCH_MOSI  32
  #define CYD_TOUCH_MISO  39
  #define CYD_TOUCH_CLK   25
  #define CYD_TOUCH_SEPARATE_SPI 1
#endif

#ifdef CYD_35
  // 3.5" SHARES SPI bus with display (resistive touch version)
  // For capacitive GT911 version, touch uses I2C instead
  #define CYD_TOUCH_IRQ   36
  #define CYD_TOUCH_MOSI  13   // Shared with TFT_MOSI
  #define CYD_TOUCH_MISO  12   // Shared with TFT_MISO
  #define CYD_TOUCH_CLK   14   // Shared with TFT_SCLK
  #define CYD_TOUCH_SEPARATE_SPI 0

  // GT911 Capacitive touch (if using C variant)
  // Uncomment these if you have the capacitive version
  //#define CYD_35_CAPACITIVE
  //#define CYD_GT911_SDA   33
  //#define CYD_GT911_SCL   32
  //#define CYD_GT911_RST   25
  //#define CYD_GT911_INT   21
#endif

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 3: FONTS
// ═══════════════════════════════════════════════════════════════════════════

#define LOAD_GLCD    // Font 1. Original Adafruit 8 pixel font
#define LOAD_FONT2   // Font 2. Small 16 pixel high font
#define LOAD_FONT4   // Font 4. Medium 26 pixel high font
#define LOAD_FONT6   // Font 6. Large 48 pixel font (numbers only)
#define LOAD_FONT7   // Font 7. 7 segment 48 pixel font (numbers only)
#define LOAD_FONT8   // Font 8. Large 75 pixel font (numbers only)
#define LOAD_GFXFF   // FreeFonts - 48 Adafruit_GFX free fonts

#define SMOOTH_FONT

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 4: SPI SETTINGS
// ═══════════════════════════════════════════════════════════════════════════

// Display SPI frequency
#ifdef CYD_28
  #define SPI_FREQUENCY  55000000  // 55MHz for ILI9341
#endif
#ifdef CYD_35
  #define SPI_FREQUENCY  40000000  // 40MHz for ST7796 (more conservative)
#endif

// Touch SPI frequency (XPT2046 requires slower speed)
#define SPI_TOUCH_FREQUENCY  2500000  // 2.5MHz

// Read frequency
#define SPI_READ_FREQUENCY  20000000

// Use HSPI port for display
#define USE_HSPI_PORT

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 5: ADDITIONAL SETTINGS
// ═══════════════════════════════════════════════════════════════════════════

// Color order - try BGR if colors look wrong
//#define TFT_RGB_ORDER TFT_RGB
//#define TFT_RGB_ORDER TFT_BGR

// Inversion - uncomment if display colors are inverted
//#define TFT_INVERSION_ON
//#define TFT_INVERSION_OFF

// ═══════════════════════════════════════════════════════════════════════════
// SECTION 6: HALEHOUND-CYD SPECIFIC PINS (from cyd_config.h)
// Defined here for reference - actual usage in cyd_config.h
// ═══════════════════════════════════════════════════════════════════════════

// These pins are the SAME on both 2.8" and 3.5" boards:
//
// SD Card (VSPI) - DISABLED, used for radios:
//   SD_CS   = GPIO 5
//   SD_SCK  = GPIO 18  → Used for CC1101/NRF24 SPI
//   SD_MISO = GPIO 19  → Used for CC1101/NRF24 SPI
//   SD_MOSI = GPIO 23  → Used for CC1101/NRF24 SPI
//
// RGB LED - DISABLED, used for NRF24:
//   RED   = GPIO 4   → NRF24_CSN
//   GREEN = GPIO 16  → NRF24_CE
//   BLUE  = GPIO 17  → NRF24_IRQ / GPS_TX
//
// Speaker - DISABLED, used for GPS:
//   SPEAKER = GPIO 26 → GPS_RX_PIN
//
// LDR Sensor (available):
//   LDR = GPIO 34 (input only)

// ═══════════════════════════════════════════════════════════════════════════
// BOARD VALIDATION
// ═══════════════════════════════════════════════════════════════════════════

#if !defined(CYD_28) && !defined(CYD_35)
  #error "You must define either CYD_28 or CYD_35 at the top of User_Setup.h"
#endif

#if defined(CYD_28) && defined(CYD_35)
  #error "You cannot define both CYD_28 and CYD_35 - choose one board"
#endif
