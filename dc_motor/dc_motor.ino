// DC Motor control with DRV8833 H-bridge + OLED display

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 5
#define OLED_SCL 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Motor pins
#define IN1 2  // GPIO2 -> DRV8833 IN1
#define IN2 3  // GPIO3 -> DRV8833 IN2

#define PWM_FREQ 20000     // 20kHz PWM frequency
#define PWM_RESOLUTION 8   // 8-bit resolution (0-255)

#define SLOW_SPEED 150     // ~60% duty cycle
#define FAST_SPEED 200     // ~80% duty cycle

void updateDisplay(const char* status, int speed) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(32, 28);
    display.print("Speed: ");
    display.print(speed);
    display.setCursor(32, 40);
    display.print(status);
    display.display();
}

void setup() {
    Serial.begin(115200);
    Serial.println("DC Motor - DRV8833 + OLED");

    // Initialize I2C for OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 failed");
    }
    display.clearDisplay();
    display.display();

    // Configure PWM on both pins
    ledcAttach(IN1, PWM_FREQ, PWM_RESOLUTION);
    ledcAttach(IN2, PWM_FREQ, PWM_RESOLUTION);

    // Start with motor off
    ledcWrite(IN1, 0);
    ledcWrite(IN2, 0);

    updateDisplay("Ready", 0);
    Serial.println("Ready");
}

void spinMotor(int speed) {
    ledcWrite(IN1, speed);
    ledcWrite(IN2, 0);
}

void stopMotor() {
    ledcWrite(IN1, 0);
    ledcWrite(IN2, 0);
}

void loop() {
    Serial.println("Slow speed");
    spinMotor(SLOW_SPEED);
    updateDisplay("SLOW", SLOW_SPEED);
    delay(2000);

    Serial.println("Fast speed");
    spinMotor(FAST_SPEED);
    updateDisplay("FAST", FAST_SPEED);
    delay(2000);
}
