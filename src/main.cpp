#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
// Set size of OLED display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Set global variables
const unsigned long intervalT1 = 250; // MPU interval
unsigned long previousT1 = 0;
const unsigned long intervalT2 = 500; // HC-SR04 interval
unsigned long previousT2 = 0;
long duration;
float distanceCm = 0.0;

// Set Pins for HC-SR04
const int trigPin1 = 18;
const int echoPin1 = 5;

Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


float getDistance(int trigPin, int echoPin) {
  float dCm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance
  dCm = duration * SOUND_SPEED/2;
  return dCm;
}

void setup() {
  // Configure the HC-SR04 pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  // Initialize Serial Terminal
  Serial.begin(115200);

   // Try to initialize the MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  } else {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  }
  
  // Try to initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Failed to find OLED display");
    while(1) {
      delay(10);
    }
  } else {
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.display();
  }
}

void loop() {
  // Updates time on every loop pass
  unsigned long currentTime = millis();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Get the current distance
  if (currentTime - previousT2 >= intervalT2) {
    distanceCm = getDistance(trigPin1, echoPin1);  
    // Update the timing for the next time around
    previousT2 = currentTime;
  }
  //Serial.println(distanceCm);
  // Output all the crap we have now...
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Distance(cm): ");
  display.print(distanceCm);
  display.setCursor(0,10);
  display.print("Rotation ");
  display.setCursor(55,10);
  display.print("X: ");
  display.print(g.gyro.x);
  display.setCursor(55,20);
  display.print("Y: ");
  display.print(g.gyro.y);
  display.setCursor(55,30);
  display.print("Z: ");
  display.print(g.gyro.z);
  display.display();
}