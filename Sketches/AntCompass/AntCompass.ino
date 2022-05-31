//
// AntCompass.ino
//
// Operate the LSM303DLHC sensor mounted on the antenna boom.
//

#include <Wire.h>
#include <LSM303.h>

LSM303 compass;

// from Calibrate.ino
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32768, -32768, -32768};

void seup() {

  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();

  /* Calibration values; the default values of +/-32767 for each axis
   * lead to an assumed magnetometer bias of 0. Use the Calibrate example
   * program to determine appropriate values for your particular unit.
   */
  compass.m_min = (LSM303::vector<int16_t>){ -333, -633, -986 };
  compass.m_max = (LSM303::vector<int16_t>){  390, -105, -622 };
}

// loop() from Calibrate.ino
void calibrate() {
}

void loop() {
}
