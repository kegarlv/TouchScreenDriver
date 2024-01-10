#pragma once

#include "ISystem.h"

#include "Configuration.h"
#include <SoftwareSerial.h>


class CommunicationSystem : public ISystem {

public:
  CommunicationSystem() {}
  void onStart() override {  }
  void onLoop() override {
  }
};