#include <ArduinoBLE.h>      // Library for BLE
#include <Wire.h>
#include <Adafruit_GFX.h>    // Required for OLED display
#include <Adafruit_SSD1306.h> // OLED library

// OLED display settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin (not used)
#define SCREEN_ADDRESS 0x3C // I2C address for the OLED

// Initialize the OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin and variable definitions
int pot_pin = A0;        // Define potentiometer pin
float p_val = 0;         // Variable to store the potentiometer value
float distance;          // Variable to store the mapped position
float vol;               // Variable to store the voltage

// BLE service and characteristic UUIDs
BLEService customService("38BB1816-10B3-B7D0-E378-EB423309BFEA"); 

BLEFloatCharacteristic distanceChar("38BB1816-10B3-B7D1-E378-EB423309BFEA", BLERead | BLENotify); // Distance characteristic
BLEIntCharacteristic gradeChar("38BB1816-10B3-B7D2-E378-EB423309BFEA", BLEWrite | BLERead);      // Grade characteristic

void setup() {
  //Serial.begin(9600);
  //while (!Serial); // Wait for Serial Monitor to open

  // Initialize BLE
  if (!BLE.begin()) {
   // Serial.println("BLE Init Failed!");
    while (1); // Halt if BLE initialization fails
  }
  BLE.setLocalName("MEDEMA");
  BLE.setAdvertisedService(customService);

  // Add characteristics to the service
  customService.addCharacteristic(distanceChar);
  customService.addCharacteristic(gradeChar);

  // Add the service to BLE and start advertising
  BLE.addService(customService);
  BLE.advertise();
  //BLE.setConnectionInterval(16, 32); // Set connection interval
  //Serial.println("BLE Ready and Advertising...");

  // Initialize OLED
   if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Initialize OLED with I2C address
  //   Serial.println("OLED Init Failed!");
   while (1); // Halt if OLED initialization fails
    delay(500);
  }
   display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(1000);
}

void loop() {
  // Listen for BLE central devices
  BLEDevice central = BLE.central();

  if (central) { // If a central device connects
    //Serial.println("Connected to central device.");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connected!");
    display.display();

    while (central.connected()) { // While the central device is connected
      // Read potentiometer value
      p_val = analogRead(pot_pin);

      // Calculate distance and voltage
      distance = (p_val / 1023.0) * 15.0; // Map to 0-15mm
      vol = (p_val / 1023.0) * 3.3;       // Voltage value

      // Determine the grade
      int grade;
      if (distance >= 8) {
        grade = 4;
      } else if (distance >= 5) {
        grade = 3;
      } else if (distance >= 3) {
        grade = 2;
      } else if (distance >= 2) {
        grade = 1;
      } else {
        grade = 0;
      }

      // Update BLE characteristics
      distanceChar.writeValue(distance);
      gradeChar.writeValue(grade);

      // Log values to Serial (for debugging)
      // Serial.print("Distance: ");
      // Serial.print(distance);
      // Serial.print(" mm, Grade: ");
      // Serial.println(grade);

      // Update OLED display
      display.clearDisplay();
      display.setTextSize(2); 
      display.setCursor(0, 0);
      //display.print("");
      display.print("Grade: ");
      display.println(grade);

      display.print(distance, 2); // Show distance with 2 decimals
      display.println("mm");

      //display.setTextSize(2); 
      //display.setCursor(0, 1);

      // display.print("Grade: ");
      // display.println(grade);

      display.display();
       delay(300); // Ensure BLE communication gets time to execute
    }
    

    // When the central device disconnects
    //Serial.println("Central device disconnected.");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Disconnected!");
    display.display();
  }
}
