#include <ArduinoBLE.h> // Library for BLE
#include <Wire.h>       //library for I2C communication      
#include <LCD_I2C.h>    // Library for I2C LCD

LCD_I2C lcd(0x27, 16, 2); // Initialize LCD with I2C address 0x27, 16 columns, 2 rows
int pot_pin = A0;         // Define potentiometer pin
float p_val = 0;          // Variable to store the potentiometer value
float distance;           // Variable to store the mapped position
float vol;                // Variable to store the voltage
//float slope = 0.25337;
//float intercept = -0.23499;
//float distance1;

// BLE service and characteristic UUIDs
BLEService customService("38BB1816-10B3-B7D0-E378-EB423309BFEA"); 

BLEFloatCharacteristic distanceChar("38BB1816-10B3-B7D1-E378-EB423309BFEA", BLERead | BLENotify); // Distance characteristic
BLEIntCharacteristic gradeChar("38BB1816-10B3-B7D2-E378-EB423309BFEA", BLEWrite | BLERead);      // Grade characteristic

void setup() {
  //Serial.begin(9600);
  //while (!Serial); // Wait for serial connection
  
  // Initialize BLE
  if (!BLE.begin()) {
   // Serial.println("Starting BLE failed!");
    while (1); // Halt if BLE initialization fails
  }

  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");
 delay(1000);
  // Set BLE device name and service
  BLE.setLocalName("MEDEMA");
  BLE.setAdvertisedService(customService);

  // Add characteristics to the service
  customService.addCharacteristic(distanceChar);
  customService.addCharacteristic(gradeChar);

  // Add the service to BLE
  BLE.addService(customService);

  // Start advertising
  BLE.advertise();
  //Serial.println("BLE device is now advertising...");
  lcd.setCursor(0, 1);
  lcd.print("BLE Advertising...");
  delay(1000);
}

void loop() {
  // check for connected to central device
  BLEDevice central = BLE.central();

  if (central) { // If a central device connects
    //Serial.print("Connected to central: ");
    //Serial.println(central.address());
    lcd.clear();
    lcd.print("Connected!");
 delay(1000);
    while (central.connected()) { // While the central device is connected
      // Read potentiometer value
      p_val = analogRead(pot_pin);

      // Calculate distance and voltage
      distance = (p_val / 1023.0) * 15.0; // Map to 0-15mm
      vol = (p_val / 1023.0) * 3.3;       // Voltage value
      //distance1 = (vol - intercept) / slope;

      // Display values on LCD
     
      lcd.setCursor(0, 0);
      lcd.print("Dist: ");
      lcd.print(distance, 2); // Show distance with 2 decimals
      lcd.print(" mm");

      // Print values to serial monitor
      // Serial.print("Distance1: ");
      // Serial.print(distance1, 4);
      // Serial.println(" mm");

      // Serial.print("Voltage: ");
      // Serial.println(vol);

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

      // Serial.print("Grade: ");
      // Serial.println(grade);

      lcd.setCursor(0, 1);
      lcd.print("Grade: ");
      lcd.print(grade);

      // Update BLE characteristics
      distanceChar.writeValue(distance); //send distance value over BLE
      gradeChar.writeValue(grade);//send grade value over BLE

      delay(500); // Wait before the next update
    }

    // When the central device disconnects
    //Serial.println("Disconnected from central.");
    lcd.clear();
    lcd.print("Disconnected!");
  }
}
//1. connect Arduino to computer with USB first; then connect to bluetooth with lightblue app
//2. unplug the usb, and connect to bluetooth again


