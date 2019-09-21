#pragma once

#include "CPlusPlus_Common.h"

namespace tekt {

  float remap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp = false);

  Vector remap(
    float value,
    float inputMin, float inputMax,
    const Vector& outputMin, const Vector& outputMax,
    bool clamp = false);

}

