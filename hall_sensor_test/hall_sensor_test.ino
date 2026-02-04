#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration (same as stepper script)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 5
#define OLED_SCL 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Hall sensor pin
#define HALL_PIN 4

void setup() {
    Serial.begin(115200);

    // Hall sensor input (module has pull-up, output goes LOW when magnet detected)
    pinMode(HALL_PIN, INPUT);

    // Initialize I2C for OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Hall Test");
    display.display();
    delay(1000);

    Serial.println("Hall sensor ready!");
}

void loop() {
    // A3144 is active LOW - outputs LOW when magnet is near
    bool magnetDetected = (digitalRead(HALL_PIN) == LOW);

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(32, 28);
    display.println("Hall Sensor");

    display.setCursor(32, 40);
    if (magnetDetected) {
        display.println("MAGNET!");
    } else {
        display.println("No magnet");
    }

    display.display();

    // Also print to serial
    Serial.println(magnetDetected ? "Magnet detected" : "No magnet");

    delay(100);
}
