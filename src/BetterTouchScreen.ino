// #define YP A3  // must be an analog pin, use "An" notation!
// #define XM A2  // must be an analog pin, use "An" notation!
// #define YM A1  // can be a digital pin
// #define XP A0  // can be a digital pin

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
