#include "TDClock.h"

namespace tekt {

  void Clock::configure(bool running, float rate) {
    _running = running;
    _rate = rate;
  }

  void Clock::update(const OP_TimeInfo& timeInfo) {
    if (!_running || _rate == 0.0)
    {
      _timeDelta = 0.0f;
      return;
    }

    auto deltaSec = timeInfo.deltaFrames / timeInfo.rate;

    _timeDelta = static_cast<float>(deltaSec * _rate);
    _localTime += _timeDelta;
  }

  void Clock::reset() {
    _timeDelta = 0;
    _localTime = 0;
  }
}

