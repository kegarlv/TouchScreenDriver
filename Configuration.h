#pragma once

namespace Configuration {

enum Pins {
  X_Positive = A0,
  X_Negative = A2,
  Y_Positive = A3,
  Y_Negative = A1,
};

const int baudRate = 9600;
const int xPlaneResistance = 800;

} // namespace Configuration
