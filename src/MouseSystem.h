#pragma once

#include "ISystem.h"

#include "Configuration.h"
#include "HID-Project.h"
#include "TouchscreenHID.h"

#include "TouchScreen.h"

#include "math.h"
#include "Utils.h"

#include "LoggerSystem.h"

#define TAG "MouseSystem"

namespace {

template <typename T>
T clamp(const T &value, const T &minValue, const T &maxValue) {
  return max(minValue, min(maxValue, value));
}

int map(int value, int minValue, int maxValue, int newMin, int newMax) {
  float normalizedValue =
      clamp((value - minValue) / float(maxValue - minValue), 0.f, 1.f);
  return newMin + (int)((newMax - newMin) * normalizedValue);
}

TSPoint normalizePoint(const TSPoint &point) {
  auto x = point.x; // range [90 .. 920]
  auto y = point.y; // range [180 .. 860]

  x = map(x, 85, 944, 0, 10000);
  y = map(y, 184, 866, 0, 10000);

  return {x, y, point.z};
}

} // namespace

Logger& operator<<(Logger &l, const TSPoint& point) {
  l << "{ .x =" << point.x << ", .y =" << point.y << "}";
  return l;
}

class MouseSystem : public ISystem {
  TouchScreen *mTouchScreen = nullptr;
  bool mIsPressed = false;

public:
  class OnEventListner {
  public:
    virtual void onNormalizedTouch(const TSPoint &point){};
    virtual void onRawTouch(const TSPoint &point){};
    virtual void onRelease(){};
  };

  MouseSystem() {
    mTouchScreen = new TouchScreen(
        Configuration::Pins::X_Positive, Configuration::Pins::Y_Positive,
        Configuration::X_Negative, Configuration::Y_Negative,
        Configuration::xPlaneResistance);
  }
  void onStart() override { AbsoluteMouse.begin(); }

  void onLoop() override {
    static TSPoint minP = {20000, 20000, 20000};
    static TSPoint maxP = {-20000, -20000, -20000};
    const auto point = mTouchScreen->getPoint();
    const bool pressed = point.z > mTouchScreen->pressureThreshhold;
    if (!pressed) {
      handleRelease();
    } else {
      handleTouch(point);
      maxP.x = max(maxP.x, point.x);
      maxP.y = max(maxP.y, point.y);
      minP.x = min(minP.x, point.x);
      minP.y = min(minP.y, point.y);

      Logger(TAG) << "Max" << maxP;
      Logger(TAG) << "Min" << minP;

    }
    
  }

  void setOnEventListner(OnEventListner *onEventListener) {
    mOnEventListener = onEventListener;
  }

  void removeEventListener() {
    Logger(TAG) << __FUNCTION__; 
    mOnEventListener = nullptr; 
  }


private:
  void handleRelease() {
    if (!mIsPressed)
      return;
    Logger(TAG) << __FUNCTION__;

    Serial.println("Released");
    mIsPressed = false;

    if (mOnEventListener) {
      mOnEventListener->onRelease();
    } else {
      // TODO: Move to default listener
      Touchscreen.releaseFinger(0);
      Touchscreen.send();
    }
  }

  void handleTouch(const TSPoint &point) {
    const auto normalizedPoint = normalizePoint(point);
    if (!mIsPressed) {
      Serial.println("Touch @ raw point" + Utils::toString(point));
      Serial.println("Touch @ normalized point" + Utils::toString(normalizedPoint));
    }

    mIsPressed = true;

    Logger(TAG) << __FUNCTION__ << mOnEventListener;

    if (mOnEventListener) {
      mOnEventListener->onRawTouch(point);
    }
    if (mOnEventListener) {
      mOnEventListener->onNormalizedTouch(normalizedPoint);
    }
    
    if (!mOnEventListener) {
      // TODO: Move to default listener
      // HID::Touchscreen.moveTo(normalizedPoint.x, normalizedPoint.y);
      Touchscreen.setFinger(0, normalizedPoint.x, normalizedPoint.y, 50);
      Touchscreen.send();
    }
  }

private:
  OnEventListner *mOnEventListener = nullptr;
};