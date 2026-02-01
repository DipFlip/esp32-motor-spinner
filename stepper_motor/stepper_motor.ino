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
#define STEPS_PER_REV 4096
#define STEP_INTERVAL_US 800  // microseconds between steps

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

// Volatile variables shared with interrupt
volatile int currentStep = 0;
volatile long totalSteps = 0;
volatile bool motorRunning = true;

// Hardware timer
hw_timer_t *timer = NULL;

// Interrupt service routine - called every STEP_INTERVAL_US
void IRAM_ATTR onTimer() {
    if (!motorRunning) return;

    currentStep++;
    if (currentStep > 7) currentStep = 0;
    totalSteps++;

    // Set coil states directly (faster than digitalWrite in ISR)
    digitalWrite(IN1, stepSequence[currentStep][0]);
    digitalWrite(IN2, stepSequence[currentStep][1]);
    digitalWrite(IN3, stepSequence[currentStep][2]);
    digitalWrite(IN4, stepSequence[currentStep][3]);
}

void stopMotor() {
    motorRunning = false;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}

void startMotor() {
    motorRunning = true;
}

void updateDisplay() {
    // Copy volatile variables
    long steps = totalSteps;

    float angle = fmod((steps * 360.0 / STEPS_PER_REV), 360.0);
    if (angle < 0) angle += 360;
    long revolutions = steps / STEPS_PER_REV;

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(32, 28);
    display.print("Angle: ");
    display.print(angle, 1);
    display.setCursor(32, 40);
    display.print("Revs:  ");
    display.print(revolutions);
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
    updateDisplay();

    // Set up hardware timer
    // Timer 0, prescaler 80 (1MHz tick = 1us per tick)
    timer = timerBegin(1000000);  // 1MHz frequency
    timerAttachInterrupt(timer, &onTimer);
    timerAlarm(timer, STEP_INTERVAL_US, true, 0);  // alarm every STEP_INTERVAL_US, auto-reload

    startMotor();
}

void loop() {
    // Just update display - motor runs independently via interrupt
    updateDisplay();
    delay(100);
}
