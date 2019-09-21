#pragma once

#include <array>
#include <initializer_list>
#include <string>
#include <utility>
#include <vector>
#include "CHOP_CPlusPlusBase.h"
#include "TDValues.h"
#include "ValueRange.h"

namespace tekt {

  /// Options for setting up any type of parameter (name and label).
  class ParOpts {
  public:
    ParOpts() = default;
    explicit ParOpts(const std::string& n) : name(n), label(n) {}
    ParOpts(std::string n, std::string l) : name(std::move(n)), label(std::move(l)) {}

    const std::string name;
    const std::string label;

  private:
    template<typename P>
    void applyTo(P& p) const {
      p.name = name.c_str();
      p.label = label.c_str();
    }

    friend class ParBuilder;
  };

  /// A menu option for a menu or string menu parameter.
  class MenuOpt {
  public:
    MenuOpt(std::string n, std::string l) : name(std::move(n)), label(std::move(l)) {}
    MenuOpt(std::string n) : name(std::move(n)), label(std::move(n)) {}

    const std::string name;
    const std::string label;
  };

  /// A set of menu options for a menu or string menu parameter.
  class MenuOpts {
  public:
    MenuOpts(std::initializer_list<MenuOpt> mOpts) : options(mOpts) {}

    const std::vector<MenuOpt> options;

  private:
    std::vector<const char*> getNameList() const {
      std::vector<const char*> names;
      for (const auto& option : options) {
        names.push_back(option.name.c_str());
      }
      return names;
    }
    std::vector<const char*> getLabelList() const {
      std::vector<const char*> labels;
      for (const auto& option: options) {
        labels.push_back(option.label.c_str());
      }
      return labels;
    }

    friend class ParBuilder;
  };

  template<typename T, std::size_t N>
  class NumericOptsArray;

  /// Settings for a numeric parameter (default value, slider range, limits).
  template<typename T>
  class NumericOpts {
  public:
    NumericOpts() = default;
    NumericOpts(T d, T low, T high, bool clampLow = false, bool clampHigh = false)
    : defVal(d), range(low, high), limit(low, high), clamp(clampLow, clampHigh) {}

    NumericOpts& withClamp(bool clampLow, T low, bool clampHigh, T high) {
      clamp.first = clampLow;
      clamp.second = clampHigh;
      limit.first = low;
      limit.second = high;
      return *this;
    }

    T defVal;

    std::pair<T, T> range = std::make_pair(static_cast<T>(0), static_cast<T>(1));
    std::pair<T, T> limit = std::make_pair(static_cast<T>(0), static_cast<T>(1));
    std::pair<bool, bool> clamp = std::make_pair(false, false);

    void applyTo(OP_NumericParameter& p, std::size_t i = 0) const {
      p.defaultValues[i] = static_cast<double>(defVal);
      p.minSliders[i] = static_cast<double>(range.first);
      p.maxSliders[i] = static_cast<double>(range.second);
      p.minValues[i] = static_cast<double>(limit.first);
      p.maxValues[i] = static_cast<double>(limit.second);
      p.clampMins[i] = clamp.first;
      p.clampMaxes[i] = clamp.second;
    }

    static NumericOptsArray<T, 2> createArray(NumericOpts<T> o1, NumericOpts<T> o2);
    static NumericOptsArray<T, 3> createArray(NumericOpts<T> o1, NumericOpts<T> o2, NumericOpts<T> o3);
    static NumericOptsArray<T, 4> createArray(NumericOpts<T> o1, NumericOpts<T> o2, NumericOpts<T> o3, NumericOpts<T> o4);
  };

  using FloatOpts = NumericOpts<float>;
  using IntOpts = NumericOpts<int>;

  template<typename T, std::size_t N>
  class NumericOptsArray {
  public:
    NumericOptsArray(std::array<NumericOpts<T>, N> opts) : options(std::move(opts)) {}
    NumericOptsArray(NumericOpts<T> o) : options(impl::arrayMaker<N>::make(o)) {}

    const NumericOpts<T>& operator[](const std::size_t i) const {
      return options[i];
    }
  private:
    void applyTo(OP_NumericParameter& p) const {
      for (std::size_t i = 0; i < N; i++) {
        options[i].applyTo(p, i);
      }
    }

    std::array<NumericOpts<T>, N> options;

    friend class ParBuilder;
  };

  template <typename T>
  NumericOptsArray<T, 2> NumericOpts<T>::createArray(NumericOpts<T> o1, NumericOpts<T> o2) {
    return { {o1, o2} };
  }

  template <typename T>
  NumericOptsArray<T, 3> NumericOpts<T>::createArray(NumericOpts<T> o1, NumericOpts<T> o2, NumericOpts<T> o3) {
    return { {o1, o2, o3} };
  }

  template <typename T>
  NumericOptsArray<T, 4> NumericOpts<T>::createArray(NumericOpts<T> o1, NumericOpts<T> o2, NumericOpts<T> o3, NumericOpts<T> o4) {
    return { {o1, o2, o3, o4} };
  }

  template<std::size_t N>
  using FloatOptsArray = NumericOptsArray<float, N>;

  template<std::size_t N>
  using IntOptsArray = NumericOptsArray<int, N>;

  /// Helper for constructing parameters on a particular parameter page.
  class ParBuilder {
  public:
    ParBuilder(OP_ParameterManager* m, std::string page) : _manager(m), _page(std::move(page)) {}

    void addToggle(const ParOpts& opts, bool defaultValue) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      p.defaultValues[0] = static_cast<double>(defaultValue);
      check(opts, _manager->appendToggle(p));
    }

    void addString(const ParOpts& opts, const std::string& defaultValue) const {
      OP_StringParameter p;
      opts.applyTo(p);
      setPage(p);
      p.defaultValue = defaultValue.c_str();
      check(opts, _manager->appendString(p));
    }

    void addStringMenu(const ParOpts& opts, const std::string& defaultValue, const MenuOpts& menuOpts) const {
      OP_StringParameter p;
      opts.applyTo(p);
      setPage(p);
      p.defaultValue = defaultValue.c_str();
      auto names = menuOpts.getNameList();
      auto labels = menuOpts.getLabelList();
      check(opts, _manager->appendStringMenu(
        p,
        static_cast<int32_t>(menuOpts.options.size()),
        names.data(),
        labels.data()));
    }

    void addMenu(const ParOpts& opts, const std::string& defaultValue, const MenuOpts& menuOpts) const {
      OP_StringParameter p;
      opts.applyTo(p);
      setPage(p);
      p.defaultValue = defaultValue.c_str();
      auto names = menuOpts.getNameList();
      auto labels = menuOpts.getLabelList();
      check(opts, _manager->appendMenu(
        p,
        static_cast<int32_t>(menuOpts.options.size()),
        names.data(),
        labels.data()));
    }

    void addPulse(const ParOpts& opts) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      check(opts, _manager->appendPulse(p));
    }

    void addFloat(const ParOpts& opts, const NumericOpts<float>& numOpts) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendFloat(p));
    }

    void addInt(const ParOpts& opts, const NumericOpts<int>& numOpts) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendInt(p));
    }

    template<std::size_t N>
    void addFloats(const ParOpts& opts, const FloatOptsArray<N>& numOpts) {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendFloat(p, N));
    }

    void addXYZ(const ParOpts& opts, const FloatOptsArray<3> & numOpts) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendXYZ(p));
    }

    void addRGBA(const ParOpts& opts, const FloatOptsArray<4>& numOpts) const {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendRGBA(p));
    }

    template<std::size_t N>
    void addInts(const ParOpts& opts, const NumericOptsArray<int, N>& numOpts) {
      OP_NumericParameter p;
      opts.applyTo(p);
      setPage(p);
      numOpts.applyTo(p);
      check(opts, _manager->appendInt(p, N));
    }

    void addCHOP(const ParOpts& opts) const {
      OP_StringParameter p;
      opts.applyTo(p);
      setPage(p);
      check(opts, _manager->appendCHOP(p));
    }
  private:
    template<typename P>
    void setPage(P& p) const {
      if (_page.empty()) {
        p.page = nullptr;
      }
      else {
        p.page = _page.c_str();
      }
    }

    static void check(const ParOpts& opts, OP_ParAppendResult res) {
      assert(res == OP_ParAppendResult::Success);
    }

    OP_ParameterManager* _manager;
    const std::string _page;
  };

  /// Base class for objects that represent a parameter (or tuplet of parameters).
  /// 
  /// This acts as a combination of the definition of the parameter and its settings
  /// and a helper for accessing and referencing the current value of the parameter.
  class Parameter {
  public:
    Parameter(std::string n, std::string l)
      : name(std::move(n)), label(std::move(l)) {}
    virtual ~Parameter() = default;
    const std::string name;
    const std::string label;
    virtual void create(ParBuilder& pars) const = 0;
    virtual void load(const OP_Inputs& inputs) = 0;

    virtual bool isPulse() const { return false; }
  };

  class BoolParameter final : public Parameter {
  public:
    BoolParameter(std::string n, std::string l, bool defVal)
      : Parameter(std::move(n), std::move(l)),
        defaultValue(defVal), value(defVal) {}

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;
    bool get() const { return value; }
  private:
    const bool defaultValue;
    bool value;
  };

  class StringParameter final : public Parameter {
  public:
    StringParameter(std::string n, std::string l, const std::string& defVal, MenuOpts menuOpts = {})
      : Parameter(std::move(n), std::move(l)),
      defaultValue(defVal), menuOptions(std::move(menuOpts)), value(defVal) {}

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;
    const std::string& get() const { return value; }
  private:
    const std::string defaultValue;
    const MenuOpts menuOptions;
    std::string value;
  };

  template <typename T>
  class NumericParameter final : public Parameter {
  public:
    NumericParameter(std::string n, std::string l, NumericOpts<T> numOpts)
      : Parameter(std::move(n), std::move(l)),
        numericOpts(numOpts), value(numOpts.defVal) {}

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;
    T get() const { return value; }
  private:
    const NumericOpts<T> numericOpts;
    T value;
  };

  using FloatParameter = NumericParameter<float>;
  using IntParameter = NumericParameter<int>;

  template <typename T>
  class ValueRangeParameter final : public Parameter {
  public:
    ValueRangeParameter(std::string n, std::string l, NumericOptsArray<T, 2> numOpts)
      : Parameter(std::move(n), std::move(l)),
        numericOpts(std::move(numOpts)),
        values{ numOpts[0].defVal, numOpts[1].defVal } {}

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;

    const tekt::ValueRange<T>& get() { return values; }
  private:
    const NumericOptsArray<T, 2> numericOpts;
    tekt::ValueRange<T> values;
  };

  class VectorParameter final : public Parameter {
  public:
    VectorParameter(std::string n, std::string l, NumericOptsArray<float, 3> numOpts)
      : Parameter(std::move(n), std::move(l)),
        numericOpts(std::move(numOpts)) {
      values.x = numericOpts[0].defVal;
      values.y = numericOpts[1].defVal;
      values.z = numericOpts[2].defVal;
    }

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;
    const Vector& get() const { return values; }
  private:
    const NumericOptsArray<float, 3> numericOpts;
    Vector values;
  };

  class RGBAColorParameter final : public Parameter {
  public:
    RGBAColorParameter(std::string n, std::string l, NumericOptsArray<float, 4> numOpts)
      : Parameter(std::move(n), std::move(l)),
        numericOpts(std::move(numOpts)) {
      values.r = numericOpts[0].defVal;
      values.g = numericOpts[1].defVal;
      values.b = numericOpts[2].defVal;
      values.a = numericOpts[3].defVal;
    }

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override;

    const Color& get() const { return values; }
  private:
    const NumericOptsArray<float, 4> numericOpts;
    Color values;
  };

  /// A Pulse parameter, which tracks the state of whether it has been
  /// triggered, and resets the state after the state is checked.
  class PulseParameter final : public Parameter {
  public:
    PulseParameter(std::string n, std::string l)
      : Parameter(std::move(n), std::move(l)) {}

    void create(ParBuilder& pars) const override;
    void load(const OP_Inputs& inputs) override {}

    void set() { state = true; }

    bool getAndReset() {
      if (!state) return false;
      state = false;
      return true;
    }
    void reset() {
      state = false;
    }

    bool isPulse() const override { return true; }
  private:
    bool state = false;
  };

  /// A pair of Vector parameters, which can define a 3D bounding box, or a
  /// min/max range of Vector values.
  class VectorRangeParameters final {
  public:
    VectorRangeParameters(VectorParameter l, VectorParameter h)
      : low(std::move(l)), high(std::move(h)) {}

    VectorParameter low;
    VectorParameter high;

    void create(ParBuilder& pars) const;
    void load(const OP_Inputs& inputs);

    ValueRange<Vector> get() const { return { low.get(), high.get() };}
  };

}

