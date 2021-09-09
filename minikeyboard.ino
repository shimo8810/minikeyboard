#include <math.h>

#include "MCP_DAC.h"

MCP4911 MCP;  // HW SPI

// freqs
constexpr double freqs[] = {
    277.183,  // C#
    311.127,  // D#
    293.665,  // D
    261.626,  // C
    329.628,  // E
    369.994,  // F#
    391.995,  // G
    349.228,  // F
    415.305,  // G#
    466.164,  // A#
    493.883,  // B
    440.000   // A
};

// log_2(55)
const double log2f0 = log(55.0) / M_LN2;

constexpr uint8_t NUM_KEY_IN = 4;
constexpr uint8_t NUM_KEY_OUT = 3;
constexpr uint8_t KEY_IN[] = {2, 3, 7, 9};
constexpr uint8_t KEY_OUT[] = {4, 5, 6};
constexpr uint8_t GATE = 8;

void setup() {
  // key matrix setup
  for (int i = 0; i < NUM_KEY_IN; ++i) {
    pinMode(KEY_IN[i], INPUT);
  }

  for (int i = 0; i < NUM_KEY_OUT; ++i) {
    pinMode(KEY_OUT[i], OUTPUT);
    digitalWrite(KEY_OUT[i], LOW);
  }

  // GATE setup
  pinMode(GATE, OUTPUT);
  digitalWrite(GATE, LOW);

  MCP.usesHWSPI();
  MCP.begin(10);
}

void loop() {
  bool pushed_sw = false;

  auto key = -1;

  for (auto i = 0; i < NUM_KEY_OUT; ++i) {
    digitalWrite(KEY_OUT[i], HIGH);
    for (auto j = 0; j < NUM_KEY_IN; ++j) {
      const auto val = digitalRead(KEY_IN[j]);
      if (val == HIGH) {
        pushed_sw = true;
        key = i * 4 + j;
        break;
      }
    }
    digitalWrite(KEY_OUT[i], LOW);
    if (pushed_sw) {
      break;
    }
  }

  if (pushed_sw) {
    // cv = log2(f/f0) = ln(f) / ln(2) - ln(f0) / ln(2), f0 = 110 / 2
    const auto cv = log(freqs[key]) / M_LN2 - log2f0;
    const auto val =
        static_cast<uint16_t>(cv * static_cast<double>(MCP.maxValue()) * 0.2);
    digitalWrite(GATE, HIGH);
    MCP.analogWrite(val, 0);

  } else {
    digitalWrite(GATE, LOW);
    MCP.analogWrite(0, 0);
  }
}
