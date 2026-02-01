#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED configuration (I2C on GPIO6=SCL, GPIO5=SDA)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 5
#define OLED_SCL 6

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Stepper motor pins (ULN2003)
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10

// 28BYJ-48 stepper: 4096 steps per revolution (half-step mode)
const int STEPS_PER_REV = 4096;
int stepDelay = 0; // ms between steps - using delayMicroseconds instead

// Half-step sequence for smoother operation
const int stepSequence[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

int currentStep = 0;

void setStep(int step) {
    digitalWrite(IN1, stepSequence[step][0]);
    digitalWrite(IN2, stepSequence[step][1]);
    digitalWrite(IN3, stepSequence[step][2]);
    digitalWrite(IN4, stepSequence[step][3]);
}

void stepMotor(int steps) {
    int direction = (steps > 0) ? 1 : -1;
    steps = abs(steps);

    for (int i = 0; i < steps; i++) {
        currentStep += direction;
        if (currentStep > 7) currentStep = 0;
        if (currentStep < 0) currentStep = 7;

        setStep(currentStep);
        delayMicroseconds(800); // 0.8ms between steps
    }
}

void stopMotor() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void updateDisplay(const char* status, int revolutions) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Stepper Motor");
    display.println("-------------");
    display.print("Status: ");
    display.println(status);
    display.print("Revs: ");
    display.println(revolutions);
    display.display();
}

void setup() {
    Serial.begin(115200);

    // Initialize motor pins
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    stopMotor();

    // Initialize I2C for OLED
    Wire.begin(OLED_SDA, OLED_SCL);

    // Initialize OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
    }

    display.clearDisplay();
    display.display();

    Serial.println("Stepper motor ready!");
    Serial.println("Commands: cw, ccw, stop, speed <1-10>");

    updateDisplay("Ready", 0);
}

int revCount = 0;

void loop() {
    // Simple demo: rotate one revolution clockwise, then counter-clockwise
    updateDisplay("CW", revCount);
    stepMotor(STEPS_PER_REV);  // One full revolution clockwise
    revCount++;

    delay(500);

    updateDisplay("CCW", revCount);
    stepMotor(-STEPS_PER_REV); // One full revolution counter-clockwise
    revCount++;

    delay(500);
}
