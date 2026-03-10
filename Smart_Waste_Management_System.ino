#include <Servo.h>

Servo servo1;

/* ---------------- PIN DEFINITIONS ---------------- */
const int trigPin = 8;
const int echoPin = 9;
const int servoPin = 7;
const int soilPin  = A0;

/* ---------------- VARIABLES ---------------- */
long duration;
int distance = 0;
int soil = 0;
int fsoil = 0;

/* ---------------- THRESHOLDS ---------------- */
// Adjust these after calibration
int maxDryValue = 45;      // >45% = Wet waste
int Ultra_Distance = 15;  // Object detection distance (cm)

/* ---------------- SERVO CALIBRATION ---------------- */
// These values depend on your servo
#define SERVO_WET     20
#define SERVO_DRY     165
#define SERVO_CENTER  95

void setup()
{
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  servo1.attach(servoPin);
  servo1.write(SERVO_CENTER);   // start in center

  Serial.println("Soil(%)   Distance(cm)   Result");
}

void loop()
{
  distance = 0;

  /* -------- ULTRASONIC SENSOR AVERAGING -------- */
  for (int i = 0; i < 2; i++)
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH, 30000);
    distance += duration * 0.034 / 2;
    delay(10);
  }

  distance = distance / 2;

  /* -------- OBJECT DETECTED -------- */
  if (distance < Ultra_Distance && distance > 1)
  {
    delay(800);

    /* -------- SOIL SENSOR AVERAGING -------- */
    fsoil = 0;   // IMPORTANT: reset before reading

    for (int i = 0; i < 3; i++)
    {
      soil = analogRead(soilPin);

      // Adjust 485 after calibration
      soil = constrain(soil, 485, 1023);

      fsoil += map(soil, 485, 1023, 100, 0);
      delay(75);
    }

    fsoil = fsoil / 3;

    /* -------- SERIAL OUTPUT -------- */
    Serial.print(fsoil);
    Serial.print("%      ");
    Serial.print(distance);
    Serial.print(" cm      ");

    /* -------- DECISION LOGIC -------- */
    if (fsoil > maxDryValue)
    {
      Serial.println("WET Waste");
      servo1.write(SERVO_WET);
      delay(3000);
    }
    else
    {
      Serial.println("DRY Waste");
      servo1.write(SERVO_DRY);
      delay(3000);
    }

    /* -------- RETURN TO CENTER -------- */
    servo1.write(SERVO_CENTER);
    delay(2000);
  }

  delay(1000);
}
