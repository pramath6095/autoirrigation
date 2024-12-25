#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Define the DHT11 pin and type
#define DHTPIN 4          // GPIO pin where DHT11 is connected
#define DHTTYPE DHT11     // Define the sensor type (DHT11)

// Initialize the DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);    // Initialize Serial communication
  Serial.println("DHT11 Sensor Initialization");
  
  dht.begin();             // Start the DHT sensor
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("°C  ");
    
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }

  delay(200);  // Wait for 2 seconds before the next reading
}
