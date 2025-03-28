#include "RFanController.h"

RFanController::RFanController(int pin) : txPin(pin), repeatTransmits(1) {
  pinMode(txPin, OUTPUT);
}

void RFanController::setRepeatTransmit(int repeats) {
  repeatTransmits = repeats;
}

void RFanController::transmitPulsePair(uint16_t highDuration, uint16_t lowDuration) {
  digitalWrite(txPin, HIGH);
  delayMicroseconds(highDuration);
  digitalWrite(txPin, LOW);
  delayMicroseconds(lowDuration);
}

void RFanController::transmitRawSequence(const uint16_t* pulses, size_t pulseCount) {
  // Use the first pulse value as the sync total.
  uint16_t syncTotal = pulses[0]; 
  uint16_t syncHigh = syncTotal / 11;    // 1:10 ratio split
  uint16_t syncLow  = syncTotal - syncHigh;
  
  // Transmit the sync pulse pair.
  transmitPulsePair(syncHigh, syncLow);
  
  // Now transmit the remaining pulses in order.
  bool level = HIGH;
  for (size_t i = 1; i < pulseCount; i++) {
    digitalWrite(txPin, level);
    delayMicroseconds(pulses[i]);
    level = !level;
  }
}

void RFanController::sendCommand(Command command) {
  // Look up the raw data for the given command.
  const uint16_t* rawData = nullptr;
  size_t pulseCount = 0;
  for (size_t i = 0; i < CMD_COUNT; i++) {
    if (commandRawMappings[i].cmd == command) {
      rawData = commandRawMappings[i].rawData;
      pulseCount = commandRawMappings[i].pulseCount;
      break;
    }
  }
  for (int rep = 0; rep < repeatTransmits; rep++) {
    transmitRawSequence(rawData, pulseCount);
  }
}

