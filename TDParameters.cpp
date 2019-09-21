#include "TDParameters.h"

namespace {
  class ParAccessor {
  public:
    explicit ParAccessor(const OP_Inputs& i) : inputs(i) {}

    bool getBool(const std::string& name) const {
      return static_cast<bool>(inputs.getParInt(name.c_str(), 0));
    }
    float getFloat(const std::string& name) const {
      return static_cast<float>(inputs.getParDouble(name.c_str(), 0));
    }
    int getInt(const std::string& name) const {
      return static_cast<int>(inputs.getParInt(name.c_str(), 0));
    }
    std::string getString(const std::string& name) const {
      return std::string(inputs.getParString(name.c_str()));
    }
    Vector getVector(const std::string& name) const {
      double x, y, z;
      inputs.getParDouble3(name.c_str(), x, y, z);
      return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
    }
    Color getRGBAColor(const std::string& name) const {
      double r, g, b, a;
      inputs.getParDouble4(name.c_str(), r, g, b, a);
      return { static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a) };
    }
    void getFloatPair(const std::string& name, float* v1, float* v2) const {
      double da, db;
      inputs.getParDouble2(name.c_str(), da, db);
      *v1 = static_cast<float>(da);
      *v2 = static_cast<float>(db);
    }
  private:
    const OP_Inputs& inputs;
  };
}

namespace tekt {

  void BoolParameter::create(ParBuilder& pars) const {
    pars.addToggle({ name, label }, defaultValue);
  }

  void BoolParameter::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    value = pars.getBool(name);
  }

  void StringParameter::create(ParBuilder& pars) const {
    if (menuOptions.options.empty()) {
      pars.addString({ name, label }, defaultValue);
    } else {
      pars.addStringMenu({ name, label }, defaultValue, menuOptions);
    }
  }

  void StringParameter::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    value = pars.getString(name);
  }

  template <>
  void NumericParameter<float>::create(ParBuilder& pars) const {
    pars.addFloat({ name, label }, numericOpts);
  }

  template <>
  void NumericParameter<int>::create(ParBuilder& pars) const {
    pars.addInt({ name, label }, numericOpts);
  }

  template <>
  void NumericParameter<float>::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    value = pars.getFloat(name);
  }

  template <>
  void NumericParameter<int>::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    value = pars.getInt(name);
  }

  template <>
  void ValueRangeParameter<float>::create(ParBuilder& pars) const {
    pars.addFloats({ name, label }, numericOpts);
  }


  template <>
  void ValueRangeParameter<float>::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    pars.getFloatPair(name, &values.low, &values.high);
  }

  void VectorParameter::create(ParBuilder& pars) const {
    pars.addXYZ({ name, label }, numericOpts);
  }

  void VectorParameter::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    values = pars.getVector(name);
  }

  void RGBAColorParameter::create(ParBuilder& pars) const {
    pars.addRGBA({ name, label }, numericOpts);
  }

  void RGBAColorParameter::load(const OP_Inputs& inputs) {
    ParAccessor pars{ inputs };
    values = pars.getRGBAColor(name);
  }

  void PulseParameter::create(ParBuilder& pars) const {
    pars.addPulse({ name, label });
  }

  void VectorRangeParameters::create(ParBuilder& pars) const {
    low.create(pars);
    high.create(pars);
  }

  void VectorRangeParameters::load(const OP_Inputs& inputs) {
    low.load(inputs);
    high.load(inputs);
  }

}
