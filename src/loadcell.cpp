#include <HX711.h>
#include <Joystick.h>

// Initialize Joystick (ID: 0, Joystick Type: Joystick)
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   false, false, false, false, false, false,
                   false, false, false, true, false); // Enable Brake axis

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

// Calibration variables (Adjust these after running a calibration test)
long minReading = 5000;    // Raw value when pedal is released
long maxReading = 400000; // Raw value at maximum foot pressure

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  // Start the joystick
  Joystick.begin();
  Joystick.setBrakeRange(0, 1023);
  
  // Optional: tare/reset scale on startup if empty
  // scale.set_scale();
  // scale.tare();
}

void loop() {
  if (scale.is_ready()) {
    long rawValue = scale.read(); // Single conversion already ready — no extra wait
    
    // Constrain the raw value to your calibrated min and max bounds
    long constrainedValue = constrain(rawValue, minReading, maxReading);
    
    // Map the raw load cell values to 0 - 1023 (standard 10-bit joystick axis)
    int mappedBrake = map(constrainedValue, minReading, maxReading, 0, 1023);
    
    // Send the value over USB to the PC as the Brake axis
    Joystick.setBrake(mappedBrake);
    
    // Debugging via Serial Monitor
    Serial.print("Raw: ");
    Serial.println(rawValue);
  } else {
    // Serial.println("HX711 not found.");
  }
}
