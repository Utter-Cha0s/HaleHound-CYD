// ═══════════════════════════════════════════════════════════════════════════
// HaleHound-CYD Utility Functions Implementation
// Button handling, display helpers, common utilities
// Created: 2026-02-06
// ═══════════════════════════════════════════════════════════════════════════

#include "utils.h"
#include "shared.h"
#include "gps_module.h"
#include <EEPROM.h>

// ═══════════════════════════════════════════════════════════════════════════
// BUTTON INPUT FUNCTIONS
// These replace the PCF8574 I2C expander from ESP32-DIV
// ═══════════════════════════════════════════════════════════════════════════

// initButtons() and updateButtons() are defined as inline in touch_buttons.h

bool isUpButtonPressed() {
    return buttonPressed(BTN_UP) || buttonHeld(BTN_UP);
}

bool isDownButtonPressed() {
    return buttonPressed(BTN_DOWN) || buttonHeld(BTN_DOWN);
}

bool isLeftButtonPressed() {
    return buttonPressed(BTN_LEFT) || buttonHeld(BTN_LEFT);
}

bool isRightButtonPressed() {
    return buttonPressed(BTN_RIGHT) || buttonHeld(BTN_RIGHT);
}

bool isSelectButtonPressed() {
    return buttonPressed(BTN_SELECT);
}

bool isBackButtonPressed() {
    return buttonPressed(BTN_BACK);
}

// isBootButtonPressed() is defined in touch_buttons.cpp

void waitForButtonPress() {
    clearButtonEvents();

    while (!anyButtonPressed()) {
        updateButtons();
        delay(20);
    }
}

void waitForButtonRelease() {
    while (anyButtonPressed()) {
        updateButtons();
        delay(20);
    }
}

bool anyButtonActive() {
    return anyButtonPressed();
}

// ═══════════════════════════════════════════════════════════════════════════
// DISPLAY HELPER FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════

void clearScreen() {
    tft.fillScreen(HALEHOUND_BLACK);
}

void drawStatusBar() {
    // GPS indicator draws directly on screen background — no bar, no wasted space
    #if CYD_HAS_GPS
    drawGPSIndicator(5, 0);
    #endif
}

void drawTitleBar(const char* title) {
    // Draw title background
    tft.fillRect(0, STATUS_BAR_HEIGHT + 1, SCREEN_WIDTH, 25, HALEHOUND_DARK);

    // Draw title text
    tft.setTextColor(HALEHOUND_HOTPINK);
    tft.setTextSize(2);

    // Center the title
    int titleWidth = strlen(title) * 12;  // Approximate width
    int x = (SCREEN_WIDTH - titleWidth) / 2;
    if (x < 5) x = 5;

    tft.setCursor(x, STATUS_BAR_HEIGHT + 5);
    tft.print(title);

    // Draw separator line
    tft.drawLine(0, STATUS_BAR_HEIGHT + 26, SCREEN_WIDTH, STATUS_BAR_HEIGHT + 26, HALEHOUND_VIOLET);
}

void drawMenuItem(int y, const char* text, bool selected) {
    if (selected) {
        // Highlighted item
        tft.fillRect(0, y, SCREEN_WIDTH, 24, HALEHOUND_MAGENTA);
        tft.setTextColor(HALEHOUND_BLACK);
    } else {
        // Normal item
        tft.fillRect(0, y, SCREEN_WIDTH, 24, HALEHOUND_BLACK);
        tft.setTextColor(HALEHOUND_CYAN);
    }

    tft.setTextSize(2);
    tft.setCursor(10, y + 4);
    tft.print(text);
}

void drawProgressBar(int x, int y, int width, int height, int percent, uint16_t color) {
    // Clamp percent
    if (percent < 0) percent = 0;
    if (percent > 100) percent = 100;

    // Draw border
    tft.drawRect(x, y, width, height, HALEHOUND_CYAN);

    // Calculate fill width
    int fillWidth = ((width - 2) * percent) / 100;

    // Draw fill
    if (fillWidth > 0) {
        tft.fillRect(x + 1, y + 1, fillWidth, height - 2, color);
    }

    // Clear rest
    if (fillWidth < width - 2) {
        tft.fillRect(x + 1 + fillWidth, y + 1, width - 2 - fillWidth, height - 2, HALEHOUND_BLACK);
    }
}

void drawCenteredText(int y, const char* text, uint16_t color, int size) {
    tft.setTextColor(color);
    tft.setTextSize(size);

    int charWidth = 6 * size;  // Approximate character width
    int textWidth = strlen(text) * charWidth;
    int x = (SCREEN_WIDTH - textWidth) / 2;
    if (x < 0) x = 0;

    tft.setCursor(x, y);
    tft.print(text);
}

// ═══════════════════════════════════════════════════════════════════════════
// GLITCH TEXT - Chromatic Aberration Effect
// Nosifer horror font + 3-pass render: cyan ghost, pink ghost, white center
// ═══════════════════════════════════════════════════════════════════════════

void drawGlitchText(int y, const char* text, const GFXfont* font) {
    tft.setFreeFont(font);
    int w = tft.textWidth(text);
    int x = (SCREEN_WIDTH - w) / 2;
    if (x < 0) x = 0;

    // Pass 1: ghost offset left-up
    tft.setTextColor(HALEHOUND_CYAN);
    tft.setCursor(x - 1, y - 1);
    tft.print(text);

    // Pass 2: ghost offset right-down
    tft.setTextColor(HALEHOUND_HOTPINK);
    tft.setCursor(x + 1, y + 1);
    tft.print(text);

    // Pass 3: white main text on top
    tft.setTextColor(TFT_WHITE);
    tft.setCursor(x, y);
    tft.print(text);

    tft.setFreeFont(NULL);
}

void drawGlitchTitle(int y, const char* text) {
    drawGlitchText(y, text, &Nosifer_Regular12pt7b);
}

void drawGlitchStatus(int y, const char* text, uint16_t color) {
    tft.setFreeFont(&Nosifer_Regular10pt7b);
    tft.setTextColor(color, TFT_BLACK);
    int w = tft.textWidth(text);
    int x = (SCREEN_WIDTH - w) / 2;
    if (x < 0) x = 0;
    tft.setCursor(x, y);
    tft.print(text);
    tft.setFreeFont(NULL);
}

// ═══════════════════════════════════════════════════════════════════════════
// GPS STATUS HELPERS
// ═══════════════════════════════════════════════════════════════════════════

void drawGPSIndicator(int x, int y) {
    #if CYD_HAS_GPS
    GPSStatus status = gpsGetStatus();

    uint16_t color;
    const char* text;

    switch (status) {
        case GPS_NO_MODULE:
            color = HALEHOUND_GUNMETAL;
            text = "GPS:--";
            break;
        case GPS_SEARCHING:
            color = HALEHOUND_HOTPINK;
            text = "GPS:..";
            break;
        case GPS_FIX_2D:
            color = HALEHOUND_BRIGHT;
            text = "GPS:2D";
            break;
        case GPS_FIX_3D:
            color = 0x07E0;  // Green
            text = "GPS:3D";
            break;
        default:
            color = RED;
            text = "GPS:??";
            break;
    }

    tft.setTextColor(color);
    tft.setTextSize(1);
    tft.setCursor(x, y + 4);
    tft.print(text);

    // If we have fix, show satellite count
    if (status == GPS_FIX_2D || status == GPS_FIX_3D) {
        tft.setCursor(x + 45, y + 4);
        tft.print(gpsGetSatellites());
        tft.print("s");
    }
    #endif
}

String getGPSStatusText() {
    #if CYD_HAS_GPS
    GPSStatus status = gpsGetStatus();

    switch (status) {
        case GPS_NO_MODULE:  return "No GPS";
        case GPS_SEARCHING:  return "Searching...";
        case GPS_FIX_2D:     return "2D Fix (" + String(gpsGetSatellites()) + " sats)";
        case GPS_FIX_3D:     return "3D Fix (" + String(gpsGetSatellites()) + " sats)";
        default:             return "Unknown";
    }
    #else
    return "GPS Disabled";
    #endif
}

// ═══════════════════════════════════════════════════════════════════════════
// STRING UTILITIES
// ═══════════════════════════════════════════════════════════════════════════

String truncateString(const String& str, int maxChars) {
    if (str.length() <= maxChars) {
        return str;
    }
    return str.substring(0, maxChars - 2) + "..";
}

String formatFrequency(float mhz) {
    char buf[16];
    sprintf(buf, "%.2f MHz", mhz);
    return String(buf);
}

String formatRSSI(int rssi) {
    char buf[16];
    sprintf(buf, "%d dBm", rssi);
    return String(buf);
}

// ═══════════════════════════════════════════════════════════════════════════
// TIMING UTILITIES
// ═══════════════════════════════════════════════════════════════════════════

bool delayWithButtonCheck(uint32_t ms) {
    uint32_t start = millis();

    while (millis() - start < ms) {
        updateButtons();

        if (anyButtonActive()) {
            return true;  // Button was pressed
        }

        delay(20);
    }

    return false;  // No button pressed
}

String getElapsedTimeString(uint32_t startMillis) {
    uint32_t elapsed = (millis() - startMillis) / 1000;

    if (elapsed < 60) {
        return String(elapsed) + "s";
    } else if (elapsed < 3600) {
        int mins = elapsed / 60;
        int secs = elapsed % 60;
        return String(mins) + "m " + String(secs) + "s";
    } else {
        int hours = elapsed / 3600;
        int mins = (elapsed % 3600) / 60;
        return String(hours) + "h " + String(mins) + "m";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// EEPROM / STORAGE HELPERS
// ═══════════════════════════════════════════════════════════════════════════

#define EEPROM_SIZE 512
#define EEPROM_MAGIC 0xCD02

// Globals defined in HaleHound-CYD.ino
extern int brightness_level;
extern int screen_timeout_seconds;

struct Settings {
    uint16_t magic;
    uint8_t brightness;
    float lastFrequency;
    uint8_t touchCalibrated;
    uint16_t touchCalData[5];
    uint16_t screenTimeout;
};

static Settings settings;

void saveSettings() {
    settings.magic = EEPROM_MAGIC;
    settings.brightness = (uint8_t)brightness_level;
    settings.screenTimeout = (uint16_t)screen_timeout_seconds;

    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(0, settings);
    EEPROM.commit();
    EEPROM.end();

    #if CYD_DEBUG
    Serial.printf("[UTILS] Settings saved (brightness=%d, timeout=%d)\n",
                  settings.brightness, settings.screenTimeout);
    #endif
}

void loadSettings() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get(0, settings);
    EEPROM.end();

    if (settings.magic != EEPROM_MAGIC) {
        // First run or corrupted - set defaults
        settings.magic = EEPROM_MAGIC;
        settings.brightness = 255;
        settings.lastFrequency = 433.92;
        settings.touchCalibrated = 0;
        settings.screenTimeout = 60;

        #if CYD_DEBUG
        Serial.println("[UTILS] No valid settings found, using defaults");
        #endif
    } else {
        // Apply saved settings to globals
        brightness_level = settings.brightness;
        screen_timeout_seconds = settings.screenTimeout;

        #if CYD_DEBUG
        Serial.printf("[UTILS] Settings loaded (brightness=%d, timeout=%d)\n",
                      settings.brightness, settings.screenTimeout);
        #endif
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// DEBUG HELPERS
// ═══════════════════════════════════════════════════════════════════════════

void printHeapStatus() {
    #if CYD_DEBUG
    Serial.println("[HEAP] Free: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("[HEAP] Min:  " + String(ESP.getMinFreeHeap()) + " bytes");
    #endif
}

void printSystemInfo() {
    #if CYD_DEBUG
    Serial.println("═══════════════════════════════════════════════════════════");
    Serial.println("              HALEHOUND-CYD SYSTEM INFO");
    Serial.println("═══════════════════════════════════════════════════════════");
    Serial.println("Board:      " + String(CYD_BOARD_NAME));
    Serial.println("Screen:     " + String(SCREEN_WIDTH) + "x" + String(SCREEN_HEIGHT));
    Serial.println("CPU Freq:   " + String(ESP.getCpuFreqMHz()) + " MHz");
    Serial.println("Flash:      " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB");
    Serial.println("Free Heap:  " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("SDK:        " + String(ESP.getSdkVersion()));
    Serial.println("───────────────────────────────────────────────────────────");
    Serial.println("Features:");
    Serial.println("  CC1101:   " + String(CYD_HAS_CC1101 ? "YES" : "NO"));
    Serial.println("  NRF24:    " + String(CYD_HAS_NRF24 ? "YES" : "NO"));
    Serial.println("  GPS:      " + String(CYD_HAS_GPS ? "YES" : "NO"));
    Serial.println("  SD Card:  " + String(CYD_HAS_SDCARD ? "YES" : "NO"));
    Serial.println("═══════════════════════════════════════════════════════════");
    #endif
}
