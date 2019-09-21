#pragma once

#include <array>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "CHOP_CPlusPlusBase.h"
#include "TDValues.h"

namespace tekt {
  template<std::size_t N>
  using InputChannelTuple = std::array<const float*, N>;

  template<std::size_t N>
  using OutputChannelTuple = std::array<float*, N>;

  class ChannelMap {
  public:
    ChannelMap() = default;
    explicit ChannelMap(std::initializer_list<std::string> names) {
      addAll(names);
    }

    ChannelMap clone() const { return ChannelMap(*this); }

    ChannelMap& add(const std::string& name);

    ChannelMap& addIfMissing(const std::string& name);

    ChannelMap& addFromInput(const OP_CHOPInput* input);

    ChannelMap& addAll(std::initializer_list<std::string> names) {
      for (const auto& name : names) {
        addIfMissing(name);
      }
      return *this;
    }

    void clear();

    const float* inputData(const OP_CHOPInput* input,
                           const std::string& name);

    float* outputData(CHOP_Output* output,
                      const std::string& name) const;

    InputChannelTuple<3> inputDataTuple(
                                        const OP_CHOPInput* input,
                                        const std::string& name1,
                                        const std::string& name2,
                                        const std::string& name3)
    {
      return {
        inputData(input, name1),
        inputData(input, name2),
        inputData(input, name3)
      };
    }

    OutputChannelTuple<3> outputDataTuple(
                                          CHOP_Output* output,
                                          const std::string& name1,
                                          const std::string& name2,
                                          const std::string& name3) const
    {
      return {
        outputData(output, name1),
        outputData(output, name2),
        outputData(output, name3)
      };
    }

    template<std::size_t N>
    InputChannelTuple<N> inputDataTuple(const OP_CHOPInput* input,
                                        const std::array<std::string, N>& names) {
      InputChannelTuple<N> tuple;
      for (auto i = 0; i < N; i++) {
        tuple[i] = inputData(input, names[i]);
      }
      return tuple;
    }

    template<std::size_t N>
    OutputChannelTuple<N> outputDataTuple(CHOP_Output* output,
                                          const std::array<std::string, N>& names) const {
      OutputChannelTuple<N> tuple;
      for (auto i = 0; i < N; i++) {
        tuple[i] = outputData(output, names[i]);
      }
      return tuple;
    }

    int32_t channelIndex(const std::string& name) const;

    int32_t channelCount() const { return static_cast<int32_t>(_orderedNames.size()); }
    const std::string& channelName(int32_t index) const { return _orderedNames[index]; }

    void getChannelName(int32_t index, OP_String* name) const;

    const std::unordered_set<std::string>& unusedInputNames() const { return _unusedInputNames; }
  private:
    std::unordered_map<std::string, int32_t> _indicesByName;
    std::vector<std::string> _orderedNames;
    std::unordered_set<std::string> _unusedInputNames;
  };

  namespace impl {

    template <typename T>
    std::enable_if_t<!std::is_enum_v<T>, T>
      getSample(const float* data, int32_t i, const T& defaultVal)
    {
      if (data == nullptr)
      {
        return defaultVal;
      }
      return static_cast<T>(data[i]);
    }

    template <typename T>
    std::enable_if_t<std::is_enum_v<T>, T>
      getSample(const float* data, int32_t i, const T& defaultVal) {
      return static_cast<T>(getSample<int>(data, i, static_cast<int>(defaultVal)));
    }

    template <typename T, std::size_t N>
    T getSample(const InputChannelTuple<N>& data, int32_t i, const std::array<T, N>& defaultVals) {
      T result;
      for (auto part = 0; part < N; part++) {
        result[part] = getSample(data[part], i, defaultVals[part]);
      }
      return result;
    }

    template <typename T>
    T getSample(const InputChannelTuple<1> & data, int32_t i, const std::array<T, 1> & defaultVals) {
      return getSample(data[0], i, defaultVals[0]);
    }

    template <typename T>
    T getSample(const InputChannelTuple<1> & data, int32_t i, const T& defaultVal) {
      return getSample(data[0], i, defaultVal);
    }

    template <typename V>
    V getSample(const InputChannelTuple<3> & data, int32_t i, const V defaultVal)
    {
      return {
        getSample(data[0], i, impl::tupleField<V, 0>(defaultVal)),
        getSample(data[1], i, impl::tupleField<V, 1>(defaultVal)),
        getSample(data[2], i, impl::tupleField<V, 2>(defaultVal)),
      };
    }

    template <typename V>
    V getSample(const InputChannelTuple<4> & data, int32_t i, const V defaultVal)
    {
      return {
        getSample(data[0], i, impl::tupleField<V, 0>(defaultVal)),
        getSample(data[1], i, impl::tupleField<V, 1>(defaultVal)),
        getSample(data[2], i, impl::tupleField<V, 2>(defaultVal)),
        getSample(data[3], i, impl::tupleField<V, 3>(defaultVal)),
      };
    }

    template <typename T>
    void setSample(float* data, int32_t i, T val) {
      if (data != nullptr) {
        data[i] = static_cast<float>(val);
      }
    }

    template <typename T>
    void setSample(const OutputChannelTuple<1> & data, int32_t i, const T& value) {
      setSample(data[0], i, value);
    }

    template <typename V>
    void setSample(const OutputChannelTuple<3> & data, int32_t i, const V& value)
    {
      setSample(data[0], i, impl::tupleField<V, 0>(value));
      setSample(data[1], i, impl::tupleField<V, 1>(value));
      setSample(data[2], i, impl::tupleField<V, 2>(value));
    }

    template <typename V>
    void setSample(const OutputChannelTuple<4> & data, int32_t i, const V& value)
    {
      setSample(data[0], i, impl::tupleField<V, 0>(value));
      setSample(data[1], i, impl::tupleField<V, 1>(value));
      setSample(data[2], i, impl::tupleField<V, 2>(value));
      setSample(data[3], i, impl::tupleField<V, 3>(value));
    }
  }

  class OutputChannelBase {
  public:
    OutputChannelBase() = default;
    virtual ~OutputChannelBase() = default;
    virtual void detach() = 0;
    virtual void attachOutput(CHOP_Output* outputs, const ChannelMap& chans) = 0;
    virtual void outputDefault(int32_t i) = 0;
  };

  template<typename T>
  class OutputChannel final : public OutputChannelBase {
  protected:
    static constexpr std::size_t N = impl::arity<T>::value;
  public:
    OutputChannel(std::array<std::string, N>&& names, T&& defaults)
    : _names(names), _defaults(defaults) {
      _output.fill(nullptr);
    }
    void detach() override {
      _output.fill(nullptr);
    }
    void attachOutput(CHOP_Output* outputs, const ChannelMap& chans) override {
      _output = chans.outputDataTuple(outputs, _names);
    }
    void output(int32_t i, const T& value) {
      assert(_output[0] != nullptr);
      impl::setSample(_output, i, value);
    }
    void outputDefault(int32_t i) override {
      output(i, _defaults);
    }
  private:
    const std::array<std::string, N> _names;
    const T _defaults;
    OutputChannelTuple<N> _output;
  };

  using FloatOutChannel = OutputChannel<float>;
  using IntOutChannel = OutputChannel<int>;
  using BoolOutChannel = OutputChannel<bool>;
  using VectorOutChannel = OutputChannel<Vector>;
  using ColorOutChannel = OutputChannel<Color>;

  class InputChannelBase {
  public:
    InputChannelBase() = default;
    virtual ~InputChannelBase() = default;
    virtual void detach() = 0;
    virtual void attachInput(const OP_CHOPInput* inputs, ChannelMap& chans) = 0;
  };

  template<typename T>
  class InputChannel final : public InputChannelBase {
  protected:
    static constexpr std::size_t N = impl::arity<T>::value;
  public:
    InputChannel(std::array<std::string, N>&& names, T&& defaults)
    : _names(names), _defaults(defaults) {
      _input.fill(nullptr);
    }
    void attachInput(const OP_CHOPInput* inputs, ChannelMap& chans) override {
      _input = chans.inputDataTuple(inputs, _names);
    }
    void detach() override {
      _input.fill(nullptr);
    }
    T input(int32_t i) const {
      if (!areAllPresent()) {
        return _defaults;
      }
      return impl::getSample(_input, i, _defaults);
    }
    bool areAllPresent() const {
      for (std::size_t i = 0; i < N; ++i) {
        if (_input[i] == nullptr) {
          return false;
        }
      }
      return true;
    }
  private:
    const std::array<std::string, N> _names;
    const T _defaults;
    InputChannelTuple<N> _input;
  };

  using FloatInChannel = InputChannel<float>;
  using IntInChannel = InputChannel<int>;
  using BoolInChannel = InputChannel<bool>;
  using VectorInChannel = InputChannel<Vector>;
  using ColorInChannel = InputChannel<Color>;
}
