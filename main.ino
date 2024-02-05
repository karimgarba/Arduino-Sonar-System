#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define ultSonicTrigPin 2
#define ultSonicEchoPin 4
#define MAX_TRACK_DISTANCE 15.00
#define SCREEN_X_CENTRE 64
#define SCREEN_Y_CENTRE 32
#define SONAR_RADIUS 48

// OLED display setup
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Servo motor setup
Servo myservo;  // create servo object to control a servo
int pos = 0;     // variable to store the servo position

void drawDisplay();
void drawSonarGrid();
void drawSweep();
void drawTarget();
long getDistance();

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);

  // Attach the servo to pin 3
  myservo.attach(3);

  // Set ultrasound trigger pin as OUTPUT and echo pin as INPUT
  pinMode(ultSonicTrigPin, OUTPUT);
  pinMode(ultSonicEchoPin, INPUT);
}

void loop() {
  // Sweep the servo from 0 to 180 degrees
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(10);
    drawDisplay();
  }

  // Sweep the servo from 180 to 0 degrees
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(10);
    drawDisplay();
  }
}

// Function to draw the sonar display on the OLED
void drawDisplay() {     
  oled.clearDisplay(); // clear display
  drawSonarGrid();
  oled.display();    
}

// Function to draw the sonar grid on the OLED
void drawSonarGrid() {
  oled.drawCircle(64, 64, 48, WHITE);
  oled.drawCircle(64, 64, 32, WHITE);
  oled.drawCircle(64, 64, 16, WHITE);
  oled.drawRect(0, 16, 128, 64, WHITE);
  drawSweep();
  oled.fillRect(0, 0, 128, 16, WHITE);
  oled.setTextSize(1);
  oled.setTextColor(2);
  oled.setCursor(5, 5);
  oled.print("deg: ");
  oled.print(pos);
  drawTarget();
}

// Function to draw the sonar sweep line on the OLED
void drawSweep() {
  float angRad = radians(pos);
  int lineX = SCREEN_X_CENTRE + (SONAR_RADIUS * cos(angRad));
  int lineY = SCREEN_HEIGHT - (SONAR_RADIUS * sin(angRad));
  oled.drawLine(SCREEN_X_CENTRE, SCREEN_HEIGHT, lineX, lineY, WHITE);
}

// Function to draw the sonar target on the OLED
void drawTarget() {
  long distance = getDistance();
  if (distance < MAX_TRACK_DISTANCE) {
    float tgtAngRad = radians(pos);
    float scaleFactor = distance / MAX_TRACK_DISTANCE; 
    int targetX = SCREEN_X_CENTRE + (SONAR_RADIUS * scaleFactor * cos(tgtAngRad));
    int targetY = SCREEN_HEIGHT - (SONAR_RADIUS * scaleFactor * sin(tgtAngRad));
    oled.fillCircle(targetX, targetY, 3, WHITE);
    oled.setCursor(90, 5);
    oled.print("dis ");
    oled.print(distance);  
  }
}

// Function to measure distance using ultrasound sensor
long getDistance() {
  long duration, distance;

  // Send ultrasound trigger pulse
  digitalWrite(ultSonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultSonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultSonicTrigPin, LOW);

  // Measure the pulse duration on the echo pin
  duration = pulseIn(ultSonicEchoPin, HIGH);

  // Calculate distance using the speed of sound
  distance = (duration / 2) * 0.0343;

  return distance;
}
