#pragma once
#include "TouchScreen.h"
using Point = TSPoint;

namespace Utils {
String toString(const TSPoint &point) {
  return "Point {.x=" + String(point.x) + ", .y=" + String(point.y) +
         ", .z=" + String(point.z) + "}";
}

} // namespace Utils