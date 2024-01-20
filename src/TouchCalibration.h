#pragma once

#include <ArduinoSTL.h>
#include <set>
#include <map>

#include "MouseSystem.h"
#include "SerialSystem.h"
#include "Utils.h"
#include "LoggerSystem.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "TouchCalibration"

class TouchCalibrationSystem {

  MouseSystem *mMouseSystem;

public:
  struct CalibrationData {};

  TouchCalibrationSystem(MouseSystem *system) : mMouseSystem(system) {
    mCalibrationListener = new CalibrationListener(this);
    Logger(TAG) << __FUNCTION__ << "created calibration listener" << mCalibrationListener;
    
  }

  enum CalibrationPoint {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
  };

  class CalibrationCommandHandler : public ICommandHandler {
    TouchCalibrationSystem *mParent;

  public:
    CalibrationCommandHandler(TouchCalibrationSystem *parent)
        : mParent(parent) {}

    virtual String handlerName() { return "calib"; }

    virtual std::set<String> availableCommands() {
      std::set<String> res;
      res.insert("help");
      res.insert("dump");
      res.insert("add");
      return res;
    }

    virtual void executeCommand(const std::vector<String> &args) {
      if (args.empty() || args[0] == "help") {
        Serial.println("Calibration subsystem:");
        Serial.println("Calibrating touchsceen :O");
      }

      if (args[0] == "add") {
        if (args.size() < 2) {
          Serial.println("Specify calibration point");
          // TODO move to enum
          Serial.println("tl, tr, bl, br");
        } else {
        const auto calibrationPoint = fromString(args[1]);
        Serial.println("Adding new calibration point for " +
                       toString(calibrationPoint));
          mParent->calibratePoint(calibrationPoint);
        }
      }
      Serial.println("=====");
    }
  };

  ICommandHandler *commandHandler() {
    return new CalibrationCommandHandler(this);
  }

private:
  void addCalibration(const TSPoint &point, CalibrationPoint calibPoint) {
    Serial.println("Added calibration for " + toString(calibPoint) + " in " + Utils::toString(point));
  }

  void calibratePoint(CalibrationPoint point) {
    Logger(TAG) << __FUNCTION__ << "Calibrating with listner" << mCalibrationListener;
    mCalibrationListener->setCalibrationPoint(point);
    mMouseSystem->setOnEventListner(mCalibrationListener);
  }

  static String toString(CalibrationPoint calibrationPoint) {
    std::map<CalibrationPoint, String> s;
    s[TopLeft] = "Top Left";
    s[TopRight] = "Top Right";
    s[BottomLeft] = "Bottom Left";
    s[BottomRight] = "Bottom Right";
    return s[calibrationPoint];
  }

  static CalibrationPoint fromString(const String &i) {
    std::map<String, CalibrationPoint> s;
    s["tl"] = TopLeft;
    s["tr"] = TopRight;
    s["bl"] = BottomLeft;
    s["br"] = BottomRight;
    return s[i];
  }

  std::map<CalibrationPoint, CalibrationData> mCalibrationData;

  class CalibrationListener : public MouseSystem::OnEventListner {
    TouchCalibrationSystem *mParent;
    TouchCalibrationSystem::CalibrationPoint mCalibrationPoint = TopLeft;

  public:
    CalibrationListener(TouchCalibrationSystem *parent) : mParent(parent) {}
  
    void setCalibrationPoint(CalibrationPoint calibrationPoint) {
      mCalibrationPoint = calibrationPoint;
    }

    virtual void onRawTouch(const TSPoint &point) override {
      Serial.println("Calibrated " + toString(mCalibrationPoint) + " @ " +
                     Utils::toString(point));
      mParent->addCalibration(point, mCalibrationPoint);
      mParent->mMouseSystem->removeEventListener();
    }
  };
  CalibrationListener *mCalibrationListener;
};