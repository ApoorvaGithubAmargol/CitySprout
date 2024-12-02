#define BLYNK_TEMPLATE_ID "TMPL3qYKAvX1D"
#define BLYNK_TEMPLATE_NAME "City Sprout"
#define BLYNK_AUTH_TOKEN "FJXAbp5mwQAHw4TvnIhE9-Q7fBrdVMLb"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Wi-Fi credentials
char ssid[] = "Sharath Teja";
char pass[] = "SHARATH 07";

// Variables to store dropdown selection
String selectedOption = "None";

// Pin configurations
#define SOIL_MOISTURE_PIN 34 // Analog pin for soil moisture
#define DHTPIN 4  // Digital pin connected to DHT sensor
#define DHTTYPE DHT22        // DHT sensor type: DHT11 or DHT22           // Digital pin connected to PIR sensor
#define BUZZER_PIN 27        // Digital pin for buzzer
#define MOTOR_PIN 15   // Pin for controlling water pump (relay)

// Pin configurations
#define TRIG_PIN 5         // GPIO pin for Trigger
#define ECHO_PIN 18        // GPIO pin for Echo
#define WATER_LEVEL_PIN 35   // Analog pin for water level sensor


// Blynk virtual pins
#define V0_SOIL_MOISTURE V0
#define V1_TEMPERATURE V1
#define V2_HUMIDITY V2
#define V3_LED_NOTIFICATION V3
#define V4_MOTOR_CONTROL V4
#define V5_ULTRASONIC V5
#define V6_DROPDOWNMENU V6
#define V7_WATERLEVEL V7
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Variables
bool motionDetected = false;

// Function to read soil moisture and control water pump
void readSoilMoisture() {
  int moistureLevel = analogRead(SOIL_MOISTURE_PIN);
  float moisturePercent = map(moistureLevel, 4095, 0, 0, 100); // Convert to percentage
  Blynk.virtualWrite(V0_SOIL_MOISTURE, moisturePercent);
}
// Function to read water level
void readWaterLevel() {
  int waterLevel = analogRead(WATER_LEVEL_PIN);
  float waterPercent = map(waterLevel, 4095, 0, 0, 100); // Convert to percentage
  Blynk.virtualWrite(V7_WATERLEVEL, waterPercent);
}


// Function to read temperature and humidity
void readTempHumidity() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V1_TEMPERATURE, temperature);
  Blynk.virtualWrite(V2_HUMIDITY, humidity);

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
}

// Function to measure distance using ultrasonic sensor
float getDistance() {
  digitalWrite(TRIG_PIN, LOW); // Ensure trigger pin is low
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); // Send 10us pulse to trigger
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration of the echo pulse
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm (speed of sound: 34300 cm/s)
  float distance = duration * 0.034 / 2;
  return distance;
}

BLYNK_WRITE(V1) {
  int selectedIndex = param.asInt(); // Get the index of the selected option
  selectedOption = param.asStr();   // Get the selected option as a string

  Serial.print("Selected Index: ");
  Serial.println(selectedIndex);

  Serial.print("Selected Option: ");
  Serial.println(selectedOption);

  // Add functionality based on selection
  if (selectedIndex == 0) {
    // Handle "Option 1"
    Serial.println("TOMATO");
    // Add code for Option 1
  } else if (selectedIndex == 1) {
    // Handle "Option 2"
    Serial.println("CHILLY");
    // Add code for Option 2
  } else if (selectedIndex == 2) {
    // Handle "Option 3"
    Serial.println("ROSE");
    // Add code for Option 3
  }
}


// Function to check for predators and notify
void checkForPredators() {
  float distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 20 && distance > 0) { // Predator detected
    digitalWrite(BUZZER_PIN, HIGH);  // Turn on buzzer
    Blynk.virtualWrite(V3_LED_NOTIFICATION, 255); // Turn on Blynk LED
    Serial.println("Predator detected! Notifying user...");
  } else { // No predator detected
    digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
    Blynk.virtualWrite(V3_LED_NOTIFICATION, 0); // Turn off Blynk LED
  }
}

// Blynk function to manually control the water pump
BLYNK_WRITE(V4_MOTOR_CONTROL) {
  int state = param.asInt(); // Get the button state from Blynk
  if (state == 0) { // If "off" is pressed
    digitalWrite(MOTOR_PIN, LOW); // Turn on the motor
    Serial.println("Pump turned OFF from Blynk.");
  } else if (state == 1) { // If "On" is pressed
    digitalWrite(MOTOR_PIN, HIGH); // Turn on the motor
    Serial.println("Pump turned ON from Blynk.");
  }
}


// Handle dropdown selection using a switch case
BLYNK_WRITE(V15) {
  int selectedPlant = param.asInt(); // Dropdown index (starts at 1)

  String selectedWater;
  String selectedNotes;

  switch (selectedPlant) {
    case 1: // Chilli
      selectedWater = "Water: 2–4 liters per plant per week";
      selectedNotes = "Notes: Water deeply 2–3 times a week, ensuring the soil remains moist but not waterlogged.";
      break;

    case 2: // Tomato
      selectedWater = "Water: 5–10 liters per plant per week";
      selectedNotes = "Notes: Tomatoes need consistent moisture, especially during flowering and fruiting. Water more frequently in hot weather.";
      break;

    case 3: // Coriander
      selectedWater = "Water: 1–2 liters per plant per week";
      selectedNotes = "Notes: Coriander prefers moderate watering, ensuring the soil stays moist without becoming soggy.";
      break;

    case 4: // Mint
      selectedWater = "Water: 4–6 liters per plant per week";
      selectedNotes = "Notes: Mint prefers consistently moist soil, so frequent watering (daily or every other day) is often required.";
      break;

    case 5: // Carrot
      selectedWater = "Water: 4–6 liters per square meter per week";
      selectedNotes = "Notes: Carrots require deep watering, but avoid overwatering which can cause root splitting.";
      break;

    case 6: // Potato
      selectedWater = "Water: 10–15 liters per plant per week";
      selectedNotes = "Notes: Potatoes need consistent moisture, especially during tuber development. Watering frequency increases during the growing season.";
      break;

    default:
      selectedWater = "Water: Not available";
      selectedNotes = "Notes: Please select a valid plant.";
      break;
  }

  // Update labels in Blynk
  Blynk.virtualWrite(V8, selectedWater); // Update water label
  Blynk.virtualWrite(V9, selectedNotes); // Update notes label

  // Debugging output
  Serial.println(selectedWater);
  Serial.println(selectedNotes);
}



void setup() {
  Serial.begin(115200);

  // Initialize sensors and pins
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(V5_ULTRASONIC, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, HIGH);// Ensure the pump is off at startup
 // Initialize pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Ensure buzzer is off at startup
  digitalWrite(BUZZER_PIN, LOW);


  dht.begin();

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Set up timer functions
  timer.setInterval(2000L, readSoilMoisture); // Every 2 seconds
  timer.setInterval(2000L, readTempHumidity); // Every 2 seconds
  timer.setInterval(500L, checkForPredators);

  //string output
   Blynk.syncVirtual(V15);
}

void loop() {
  Blynk.run();
  timer.run();
}
