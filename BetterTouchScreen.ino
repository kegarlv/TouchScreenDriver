// // Touch screen library with X Y and Z (pressure) readings as well
// // as oversampling to avoid 'bouncing'
// // This demo code returns raw readings, public domain

// #include <stdint.h>
// #include "TouchScreen.h"
// #include "HID-Project.h"

// #include <string.h>

// #define YP A3  // must be an analog pin, use "An" notation!
// #define XM A2  // must be an analog pin, use "An" notation!
// #define YM A1  // can be a digital pin
// #define XP A0  // can be a digital pin

// // For better pressure precision, we need to know the resistance
// // between X+ and X- Use any multimeter to read it
// // For the one we're using, its 300 ohms across the X plate
// TouchScreen ts = TouchScreen(XP, YP, XM, YM, 800);

// void setup(void) {
//   Serial.begin(9600);
//   AbsoluteMouse.begin();
// }

// TSPoint transform(const TSPoint& point) {
//   auto x  = point.x; // range [90 .. 920]
//   auto y = point.y; // range [180 .. 860]

//   return {x,y, point.z};
// }

// String toString(const TSPoint& point) {
//   return "Point {.x=" + String(point.x) + ", .y=" + String(point.y) + "}";
// }

// void loop(void) {
//   // a point object holds x y and z coordinates
//   TSPoint p = ts.getPoint();

//   // we have some minimum pressure we consider 'valid'
//   // pressure of 0 means no pressing!

//     // Serial.print("X = ");
//     // Serial.println();
//     // Serial.print(p.x);
//     // Serial.print("\tY = ");
//     // Serial.print(p.y);
//     // Serial.print("\tPressure = ");
//     // Serial.println(p.z);

//   if (p.z > ts.pressureThreshhold) {
//     Serial.println("Raw data: ");
//     Serial.println(toString(p));

//     auto modifiedP = transform(p);

//     Serial.println("Modified data: ");
//     Serial.println(toString(modifiedP));

//     AbsoluteMouse.moveTo(modifiedP.x ,
//                          modifiedP.y);
//   //   if (AbsoluteMouse.isPressed() == false) {
//   //     // AbsoluteMouse.press();
//   //   }
//   // } else {
//   //   if (AbsoluteMouse.isPressed()) {
//   //     // AbsoluteMouse.release();
//   //   }
//   }
//   delay(100);
// }

#include "MouseSystem.h"
#include "SerialSystem.h"
#include "TouchCalibration.h"

std::vector<ISystem *> mSystems;

void setup() {
  auto serialSystem = new SerialSystem();
  auto mouseSystem = new MouseSystem();
  auto calibrationSystem = new TouchCalibrationSystem(mouseSystem);

  // Add command handlers
  serialSystem->addCommandHandler(calibrationSystem->commandHandler());

  mSystems.push_back(serialSystem);
  mSystems.push_back(mouseSystem);
  

  for (auto &system : mSystems) {
    system->onStart();
  }
}

void loop() {
  for (auto &system : mSystems) {
    system->onLoop();
  }
  delay(100);
}
