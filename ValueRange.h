#pragma once

#include <algorithm>
#include <utility>
#include "TektCommon.h"

namespace tekt {

  template<typename T>
  class ValueRange {
  public:
    ValueRange() = default;
    ValueRange(T l, T h) : low(std::move(l)), high(std::move(h)) {}

    T low;
    T high;

    bool checkAndNormalize(const T& value, T* result) const
    {
      if (value < low || value > high)
      {
        return false;
      }
      *result = tekt::remap(value, low, high, static_cast<T>(0), static_cast<T>(1), true);
      return true;
    }

    T mapNormalized(T normVal, bool clamp = false) const
    {
      if (clamp) {
        normVal = std::max(normVal, static_cast<T>(0));
        normVal = std::min(normVal, static_cast<T>(1));
      }
      return low + (high - low) * normVal;
    }
  };
}
