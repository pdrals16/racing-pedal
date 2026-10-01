#include <HX711.h>
#include <Joystick.h>

// Initialize Joystick (ID: 0, Joystick Type: Joystick)
// Axes enabled: Brake (load cell) + Accelerator (Hall sensor)
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 0, 0,
                   false, false, false, false, false, false,
                   false, false, true, true, false); // Accelerator + Brake axes

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// Hall sensor
#define Hall_Sensor_Pin A0

HX711 scale;

// Load cell calibration (adjust after running a calibration test)
long loadCellMin = 5000;    // Raw value when pedal is released
long loadCellMax = 400000;  // Raw value at maximum foot pressure

// Hall sensor calibration (adjust using the "Hall:" value in the Serial Monitor)
int hallMin = 535;         // analogRead value when pedal is released
int hallMax = 730;         // analogRead value when pedal is fully pressed
const bool HALL_INVERT = false; // set to true if the axis moves backwards

// Serial debug throttle (so printing doesn't slow the loop)
unsigned long lastPrint = 0;
const unsigned long PRINT_INTERVAL_MS = 100;

void setup() {
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  pinMode(Hall_Sensor_Pin, INPUT);

  // Start the joystick
  Joystick.begin();
  Joystick.setBrakeRange(0, 1023);
  Joystick.setAcceleratorRange(0, 1023);

  // Optional: tare/reset scale on startup if empty
  // scale.set_scale();
  // scale.tare();
}

void loop() {
  static long LoadCellValue = 0;

  // --- Load cell -> Brake axis ---
  if (scale.is_ready()) {
    LoadCellValue = scale.read(); // Single conversion already ready — no extra wait

    // Constrain the raw value to your calibrated min and max bounds
    long constrainedLoadCellValue = constrain(LoadCellValue, loadCellMin, loadCellMax);

    // Map the raw load cell values to 0 - 1023 (standard 10-bit joystick axis)
    int mappedBrake = map(constrainedLoadCellValue, loadCellMin, loadCellMax, 0, 1023);

    // Send the value over USB to the PC as the Brake axis
    Joystick.setBrake(mappedBrake);
  }

  // --- Hall sensor -> Accelerator axis ---
  // Read every loop (not tied to the HX711 timing, no delay() needed)
  int hallRaw = analogRead(Hall_Sensor_Pin);
  int hallConstrained = constrain(hallRaw, hallMin, hallMax);
  int mappedHall = HALL_INVERT
                     ? map(hallConstrained, hallMin, hallMax, 1023, 0)
                     : map(hallConstrained, hallMin, hallMax, 0, 1023);

  Joystick.setAccelerator(mappedHall);

  // --- Debugging via Serial Monitor ---
  if (millis() - lastPrint >= PRINT_INTERVAL_MS) {
    lastPrint = millis();
    Serial.print("Load Cell Value: ");
    Serial.print(LoadCellValue);
    Serial.print("  Hall Value: ");
    Serial.println(hallRaw);
  }
}