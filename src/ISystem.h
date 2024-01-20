#pragma once

class ISystem {
public:
  virtual void onStart() = 0;
  virtual void onLoop() = 0;
};