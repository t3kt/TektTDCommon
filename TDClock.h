#pragma once

#include "CHOP_CPlusPlusBase.h"

namespace tekt {
  class Clock
  {
  public:
    void configure(bool running, float rate);

    void update(const OP_TimeInfo& timeInfo);
    void reset();

    bool running() const { return _running; }
    float localTime() const { return _localTime; }
    float timeDelta() const { return _timeDelta; }
  private:
    float _localTime = 0.0f;
    float _timeDelta = 0.0f;
    bool _running = false;
    float _rate = 1.0f;
  };
}
