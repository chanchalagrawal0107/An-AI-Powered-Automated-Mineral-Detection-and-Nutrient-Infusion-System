#include <DHT.h>


// ⚙️ Pin Definitions

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_PIN A0
#define TDS_PIN A1
#define PH_PIN A2
#define TRIG_PIN 9
#define ECHO_PIN 8
#define RELAY_PIN 6   // 🔌 Relay connected to digital pin 6


// ⚙️ Constants

float aref = 5.0;   // Analog reference voltage

// Moisture thresholds (tune these)
const int DRY_THRESHOLD = 40;    // below 40% -> turn ON pump
const int WET_THRESHOLD = 70;    // above 70% -> turn OFF pump

void setup() {
  Serial.begin(9600);
  dht.begin();
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  digitalWrite(RELAY_PIN, HIGH);  // Relay OFF initially (Active LOW)
}

void loop() {
  // 🌱 Soil Moisture

  int soilValue = analogRead(SOIL_PIN);
  int soilPercent = map(soilValue, 1023, 0, 0, 100); // 0% dry to 100% wet


  // 💧 TDS Sensor
  int tdsValue = analogRead(TDS_PIN);
  float tdsVoltage = tdsValue * (aref / 1023.0);
  float tds = (133.42 * tdsVoltage * tdsVoltage * tdsVoltage
              - 255.86 * tdsVoltage * tdsVoltage
              + 857.39 * tdsVoltage) * 0.5; // ppm approx


  // ⚗ pH Sensor
  int phValue = analogRead(PH_PIN);
  float phVoltage = phValue * (5.0 / 1023.0);
  float pH = 7 + ((2.5 - phVoltage) / 0.18); // calibration required


  // 🌡 Temp & Humidity
  float h = dht.readHumidity();
  float t = dht.readTemperature();


  // 📏 Ultrasonic (Water Level)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2; // cm


  // ⚡ Relay Control Logic
  if (soilPercent < DRY_THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW);   // Relay ON (Active LOW)
    Serial.println("💧 Pump ON (Soil Dry)");
  } 
  else if (soilPercent > WET_THRESHOLD) {
    digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
    Serial.println("🪴 Pump OFF (Soil Moist)");
  }


  // 📤 Print all values
  Serial.println("========== SENSOR READINGS ==========");
  Serial.print("Soil Moisture: "); Serial.print(soilPercent); Serial.println(" %");
  Serial.print("TDS: "); Serial.print(tds); Serial.println(" ppm");
  Serial.print("pH: "); Serial.println(pH);
  Serial.print("Temperature: "); Serial.print(t); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(h); Serial.println(" %");
  Serial.print("Water Level (Ultrasonic): "); Serial.print(distance); Serial.println(" cm");
  Serial.println("====================================");
  Serial.println();

  delay(2000); // Read every 2 sec
}