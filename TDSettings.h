#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "CHOP_CPlusPlusBase.h"
#include "TDParameters.h"
#include "TDValues.h"

namespace tekt {
  class ParamGroup {
  public:
    explicit ParamGroup(std::string page)
      : _page(std::move(page)) {}
    virtual ~ParamGroup() = default;
    virtual void create(OP_ParameterManager* parManager);
    virtual void load(const OP_Inputs& inputs);
  protected:
    void add(Parameter& par);
    void add(VectorRangeParameters& pars) {
      add(pars.low);
      add(pars.high);
    }
  private:
    const std::string _page;
    std::vector<Parameter*> _params;
    std::unordered_map<std::string, PulseParameter*> _pulses;

    friend class Settings;
  };

  class Settings {
  public:
    virtual ~Settings() = default;
    virtual void create(OP_ParameterManager* parManager);
    virtual void load(const OP_Inputs& inputs);
    bool handlePulse(const char* name);
    void resetPulses();
  protected:
    void add(ParamGroup& group);
  private:
    std::vector<ParamGroup*> _groups;
    std::unordered_map<std::string, PulseParameter*> _pulses;
  };

}
