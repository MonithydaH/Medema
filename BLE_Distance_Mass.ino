#include <ArduinoBLE.h>      // Library for BLE
#include <Wire.h>             // I2C communication for OLED
#include <Adafruit_GFX.h>    // Required for OLED display
#include <Adafruit_SSD1306.h> // OLED library

// OLED display settings
#define SCREEN_WIDTH 128  //128pixels wide
#define SCREEN_HEIGHT 32  // 32 pixels tall
#define OLED_RESET -1    
#define SCREEN_ADDRESS 0x3C 

// Calibration coefficients from MATLAB
#define a_s 194460
#define b_s -0.665
#define a  697476.0143
#define b -1.0206

// Initialize the OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin and variable definitions
int pot_pin = A1;        // Define potentiometer pin
float p_val = 0;         // Variable to store the potentiometer value
float distance;          // Mapped position
float vol;               // Voltage
float distance1;
const int fsrPin = A0;   // Analog pin connected to the FSR
int fsrValue = 0;
const float fixedResistance = 3300.0;  // Fixed resistor value in ohms 
const float Vcc = 3.3;  // Input voltage from Arduino
float voltage;
float fsrResistance;
float mass;
float mass_sheet;

const int redPin = 8;    
const int greenPin = 9;  
const int bluePin = 10;  

// BLE service and characteristic UUIDs
BLEService customService("38BB1816-10B3-B7D0-E378-EB423309BFEA"); 

BLEFloatCharacteristic distanceChar("38BB1816-10B3-B7D1-E378-EB423309BFEA", BLERead | BLENotify);
BLEIntCharacteristic gradeChar("38BB1816-10B3-B7D2-E378-EB423309BFEA", BLEWrite | BLERead);
BLEFloatCharacteristic massChar("38BB1816-10B3-B7D3-E378-EB423309BFEA", BLERead | BLENotify);

bool ble_connected = false;  // Track BLE connection status

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  digitalWrite(redPin, HIGH);
  digitalWrite(greenPin, HIGH);
  digitalWrite(bluePin, HIGH);

  //Serial.begin(115200);

  // Initialize BLE
  if (!BLE.begin()) {
    while (1);
  }
  
  BLE.setLocalName("Medema1");
  BLE.setAdvertisedService(customService);

  customService.addCharacteristic(distanceChar);
  customService.addCharacteristic(gradeChar);
  customService.addCharacteristic(massChar);

  BLE.addService(customService);
  BLE.advertise();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { 
    while (1);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Initializing...");
  display.display();
  delay(500);
}

void loop() {
  // Listen for BLE connection
  BLEDevice central = BLE.central();
  ble_connected = central;  // Track connection status

  // Always collect and display data (even when disconnected)
  collectData();
  displayData();

  // Send data only when BLE is connected
  if (ble_connected) {
    distanceChar.writeValue(distance);
    gradeChar.writeValue(getGrade());
    massChar.writeValue(mass);
  }

  delay(500);
}

void collectData() {
  // Read potentiometer and FSR values
  p_val = analogRead(pot_pin);
  fsrValue = analogRead(fsrPin);

  // Calculate distance and voltage
  distance = (p_val / 4095.0) * 15.0;       // Map to 0-15mm
  vol = (p_val / 4095.0) * Vcc;             
  //distance = distance1 - 0.34;
  voltage = Vcc * (fsrValue / 4095.0);       
  fsrResistance = fixedResistance * ((Vcc / voltage) - 1);

  mass = pow((fsrResistance / a), (1.0 / b));  
  mass_sheet = pow((fsrResistance / a_s), (1.0 / b_s));  
}

void displayData() {
  int grade = getGrade();

  // LED behavior based on force
  if (mass == 0 || fsrValue < 100) {
    digitalWrite(redPin, HIGH);
    digitalWrite(greenPin, HIGH);
    digitalWrite(bluePin, HIGH);
  } 
  else if (mass > 0 && mass < 500) {
    digitalWrite(bluePin, LOW);    
    digitalWrite(greenPin, HIGH);   
    digitalWrite(redPin, HIGH);    
  } 
  else {
    digitalWrite(redPin, LOW);     
    digitalWrite(bluePin, HIGH);    
    digitalWrite(greenPin, HIGH);  
  }

  // Update OLED display
  display.clearDisplay();
  display.setTextSize(2); 
  display.setCursor(0, 0);
  display.print("Grade: ");
  display.println(grade);
  display.print(distance, 2);  // Show distance with 2 decimals
  display.println(" mm");

  if (ble_connected) {
    display.println("BLE: Connected");
  } else {
    display.println("BLE: Disconnected");
  }

  display.display();
}

int getGrade() {
  if (distance >= 8) {
    return 4;
  } else if (distance >= 5) {
    return 3;
  } else if (distance >= 3) {
    return 2;
  } else if (distance >= 2) {
    return 1;
  } else {
    return 0;
  }
}
