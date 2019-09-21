#include "TDSettings.h"

namespace tekt {

  void ParamGroup::add(Parameter& par) {
    _params.push_back(&par);
    if (par.isPulse()) {
      _pulses[par.name] = dynamic_cast<PulseParameter*>(&par);
    }
  }

  void ParamGroup::create(OP_ParameterManager* parManager) {
    ParBuilder pars{ parManager, _page };
    for (auto& par : _params) {
      par->create(pars);
    }
  }

  void ParamGroup::load(const OP_Inputs& inputs) {
    for (auto& par : _params) {
      par->load(inputs);
    }
  }

  void Settings::add(ParamGroup& group) {
    _groups.push_back(&group);
    _pulses.insert(group._pulses.begin(), group._pulses.end());
  }

  void Settings::create(OP_ParameterManager* parManager) {
    for (auto& group : _groups) {
      group->create(parManager);
    }
  }

  void Settings::load(const OP_Inputs& inputs) {
    for (auto& group : _groups) {
      group->load(inputs);
    }
  }

  bool Settings::handlePulse(const char* name) {
    auto iter = _pulses.find(name);
    if (iter == _pulses.end()) {
      return false;
    }
    iter->second->set();
    return true;
  }

  void Settings::resetPulses() {
    for (auto& entry : _pulses) {
      entry.second->reset();
    }
  }

}
