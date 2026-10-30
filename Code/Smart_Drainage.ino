#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- PIN CONNECTIONS ----------

#define TRIG_PIN 5
#define ECHO_PIN 18

#define PH_PIN 34
#define RAIN_PIN 35

#define RELAY_PIN 23

#define SDA_PIN 21
#define SCL_PIN 22

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);


// ---------- SETUP ----------

void setup()
{
  Serial.begin(115200);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Relay
  pinMode(RELAY_PIN, OUTPUT);

  // Pump OFF at starting
  digitalWrite(RELAY_PIN, HIGH);

  // LCD
  Wire.begin(SDA_PIN, SCL_PIN);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("SMART DRAINAGE");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM START");

  delay(2000);
  lcd.clear();
}


// ---------- ULTRASONIC ----------

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return -1;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}


// ---------- MAIN PROGRAM ----------

void loop()
{
  // Water level
  float distance = getDistance();

  // pH sensor
  int phRaw = analogRead(PH_PIN);

  // Rain sensor
  int rainRaw = analogRead(RAIN_PIN);


  // ---------- RAIN VALUE ----------

  int rainPercent = map(
    rainRaw,
    4095,
    0,
    0,
    100
  );

  rainPercent = constrain(
    rainPercent,
    0,
    100
  );


  // ---------- pH VALUE ----------

  float voltage =
    (phRaw * 3.3) / 4095.0;

  // Approximate pH
  float pH =
    7.0 + ((2.50 - voltage) / 0.18);


  // ---------- PUMP CONTROL ----------

  // If water level is high
  if (distance > 0 && distance <= 10)
  {
    digitalWrite(RELAY_PIN, LOW);

    Serial.println("HIGH WATER LEVEL");
    Serial.println("PUMP ON");
  }
  else
  {
    digitalWrite(RELAY_PIN, HIGH);

    Serial.println("WATER LEVEL NORMAL");
    Serial.println("PUMP OFF");
  }


  // ---------- LCD DISPLAY ----------

  lcd.clear();

  lcd.setCursor(0, 0);

  if (distance < 0)
  {
    lcd.print("Water: ERROR");
  }
  else
  {
    lcd.print("Water:");
    lcd.print(distance, 1);
    lcd.print("cm");
  }

  lcd.setCursor(0, 1);

  lcd.print("pH:");
  lcd.print(pH, 1);

  lcd.print(" R:");
  lcd.print(rainPercent);
  lcd.print("%");


  // ---------- SERIAL MONITOR ----------

  Serial.print("Water Level Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("pH Raw: ");
  Serial.println(phRaw);

  Serial.print("pH Value: ");
  Serial.println(pH);

  Serial.print("Rain Raw: ");
  Serial.println(rainRaw);

  Serial.print("Rain: ");
  Serial.print(rainPercent);
  Serial.println("%");

  Serial.println("----------------------");

  delay(2000);
}
