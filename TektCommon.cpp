#include "TektCommon.h"
#include <cfloat>
#include <cmath>

namespace tekt {

  float remap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {

    if (std::fabs(inputMin - inputMax) < FLT_EPSILON) {
      return outputMin;
    }
    auto outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

    if (clamp) {
      if (outputMax < outputMin) {
        if (outVal < outputMax)outVal = outputMax;
        else if (outVal > outputMin)outVal = outputMin;
      }
      else {
        if (outVal > outputMax)outVal = outputMax;
        else if (outVal < outputMin)outVal = outputMin;
      }
    }
    return outVal;
  }

  Vector remap(float value, float inputMin, float inputMax, const Vector& outputMin, const Vector& outputMax,
    bool clamp) {
    return {
      remap(value, inputMin, inputMax, outputMin.x, outputMax.x, clamp),
      remap(value, inputMin, inputMax, outputMin.y, outputMax.y, clamp),
      remap(value, inputMin, inputMax, outputMin.z, outputMax.z, clamp)
    };
  }
}
