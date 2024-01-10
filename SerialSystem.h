#pragma once

#include "Configuration.h"
#include "ISystem.h"
#include <ArduinoSTL.h>
#include <set>

namespace {
std::vector<String> split(const String &s, const String &delimiter) {
  std::vector<String> res;
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  String token;
  while ((pos_end = s.indexOf(delimiter, pos_start)) != -1) {
    token = s.substring(pos_start, pos_end);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }
  res.push_back(s.substring(pos_start));
  return res;
}
} // namespace

class ICommandHandler {
public:
  virtual String handlerName() = 0;
  virtual std::set<String> availableCommands() = 0;
  virtual void executeCommand(const std::vector<String> &args) = 0;
};

class SerialSystem : public ISystem {
private:
  std::vector<ICommandHandler *> mCommandHandlers;
  class HelpCommand : public ICommandHandler {
    SerialSystem *mParent;

  public:
    HelpCommand(SerialSystem *parent) : mParent(parent) {}
    String handlerName() { return "help"; }
    std::set<String> availableCommands() {
      std::set<String> commands;
      commands.insert("me");
      commands.insert("pls");
      return commands;
    }

    virtual void executeCommand(const std::vector<String> &args) {
      Serial.println("Help for TouchScreen Shell");
      Serial.println("Args supplied: ");
      for (const auto &arg : args) {
        Serial.println(" - " + arg);
      }

      Serial.println("Available commands: ");
      for (const auto &h : mParent->mCommandHandlers) {
        Serial.println(" - " + h->handlerName());
        for (const auto &c : h->availableCommands()) {
          Serial.println("   + " + c);
        }
      }
    }
  };

public:
  SerialSystem() { mCommandHandlers.push_back(new HelpCommand(this)); }

  void addCommandHandler(ICommandHandler *commandHandler) {
    mCommandHandlers.push_back(commandHandler);
  }
  void onStart() override { Serial.begin(Configuration::baudRate); }

  void onLoop() override {
    if (!Serial.available()) {
      return;
    }

    const auto input = split(Serial.readString(), " ");
    auto handlerName = input[0];
    input.erase(input.begin());
    findCommandHandler(handlerName)->executeCommand(input);
  }

private:
  ICommandHandler *findCommandHandler(const String &handlerName) {
    for (const auto &handler : mCommandHandlers) {
      if (handler->handlerName() == handlerName)
        return handler;
    }
    return mCommandHandlers.front();
  }
};