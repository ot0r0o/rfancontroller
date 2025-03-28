#ifndef RFANCONTROLLER_H
#define RFANCONTROLLER_H

#include <Arduino.h>

// ------------------------------------------------------------------
// COMMAND ENUMERATION AND RAW DATA MAPPING STRUCTURES
// ------------------------------------------------------------------

enum Command {
  CMD_NONE,
  CMD_ON_OFF,
  CMD_LIGHT_ON_OFF,
  CMD_1,
  CMD_2,
  CMD_3,
  CMD_4,
  CMD_5,
  CMD_FR,
  CMD_1H,
  CMD_4H,
  CMD_8H,
  CMD_COUNT
};

// This structure maps a command to its raw pulse sequence.
struct CommandRawMapping {
  Command cmd;
  const uint16_t* rawData; 
  size_t pulseCount;
};

// Raw data arrays for each command. Normally for a ASK/OOK signal the first value is the sync burst followed by the command. 
// Replace it with your own raw data and commands.
static const uint16_t raw_ON_OFF[N] = {
  1, ...
};

static const uint16_t raw_LIGHT_ON_OFF[N] = {
  1, ...
};

static const uint16_t raw_1[N] = {
  1, ...
};

static const uint16_t raw_2[N] = {
  1, ...
};

static const uint16_t raw_3[N] = {
  1, ...
};

static const uint16_t raw_4[N] = {
  1, ...
};

static const uint16_t raw_5[N] = {
  1, ...
};

static const uint16_t raw_FR[N] = {
  1, ...
};

static const uint16_t raw_1H[N] = {
  1, ...
};

static const uint16_t raw_4H[N] = {
  1, ...
};

static const uint16_t raw_8H[N] = {
  1, ...
};

// Mapping array that associates each command with its raw data and pulse count.
static const CommandRawMapping commandRawMappings[CMD_COUNT] = {
  { CMD_ON_OFF,       raw_ON_OFF,       sizeof(raw_ON_OFF)/sizeof(uint16_t) },
  { CMD_LIGHT_ON_OFF, raw_LIGHT_ON_OFF, sizeof(raw_LIGHT_ON_OFF)/sizeof(uint16_t) },
  { CMD_1,            raw_1,            sizeof(raw_1)/sizeof(uint16_t) },
  { CMD_2,            raw_2,            sizeof(raw_2)/sizeof(uint16_t) },
  { CMD_3,            raw_3,            sizeof(raw_3)/sizeof(uint16_t) },
  { CMD_4,            raw_4,            sizeof(raw_4)/sizeof(uint16_t) },
  { CMD_5,            raw_5,            sizeof(raw_5)/sizeof(uint16_t) },
  { CMD_FR,           raw_FR,           sizeof(raw_FR)/sizeof(uint16_t) },
  { CMD_1H,           raw_1H,           sizeof(raw_1H)/sizeof(uint16_t) },
  { CMD_4H,           raw_4H,           sizeof(raw_4H)/sizeof(uint16_t) },
  { CMD_8H,           raw_8H,           sizeof(raw_8H)/sizeof(uint16_t) }
};

// ------------------------------------------------------------------
// CLASS DEFINITION
// ------------------------------------------------------------------
class RFanController {
  public:
    // Constructor: specify the TX pin.
    RFanController(int pin);
    
    // Set the number of times the entire raw sequence will be sent.
    void setRepeatTransmit(int repeats);
    
    // Send a command by replaying its stored raw data.
    void sendCommand(Command command);

  private:
    int txPin;
    int repeatTransmits;
    
    // Low-level helper: Transmit a pulse pair (first HIGH then LOW) with the given durations.
    void transmitPulsePair(uint16_t highDuration, uint16_t lowDuration);
    
    // Transmit the entire raw sequence.
    // The raw sequence is assumed to be arranged as follows:
    //   - pulses[0] is the total sync value.
    //   - pulses[1] to pulses[pulseCount-1] are the data pulses (arranged consecutively).
    // For the sync, we compute:
    //     syncHigh = pulses[0] / 11  (≈1/11 of the total)
    //     syncLow  = pulses[0] - syncHigh  (≈10/11 of the total)
    void transmitRawSequence(const uint16_t* pulses, size_t pulseCount);
};

#endif  // RFANCONTROLLER_H

