# TektTDCommon

TektTDCommon is a collection of utilities that can be used when creating TouchDesigner C++ custom OPs.

It includes utilities for:
* Parameters
* CHOP channels
* Time

## Parameters

The library provides a set of classes to represent parameters, combining their definitions and properties
with helpers for loading their values.

Each parameter *tuplet* (a single row in a parameter editor) is represented by an instance of one of the
`Parameter` classes. The constructors for these classes take in the various properties (name, label,
default value, range) that will be used when the OP's parameters are set up.

```c++
FloatParameter amount = {"Amount", "Amount of stuff", NumericOpts<float>(3.2f, 0f, 10f)};
BoolParameter enable = {"Enable", "Enable the stuff", true};
PulseParameter reset = {"Reset", "Reset the stuff"};

ParameterBase* params[] = {&amount, &enable, &reset};

// Setting up parameters
ParBuilder parBuilder { parManager, "Stuff Settings" };
for (auto par : params) {
  par->create(parBuilder);
}

// Loading their values
for (auto par : params) {
  par->load(opInputs);
}

// Using their values
if (reset.getAndReset()) {
  doReset();
}
if (enable.get()) {
  float amt = amount.get();
}
```

### Settings and ParamGroup classes

Where the `Parameter` classes represent an individual parameter, the `ParamGroup` and `Settings` classes represent groups of related parameters.

A `ParamGroup` is a group of parameters, which contains one or more parameters. A `Settings` object is a collection of `ParamGroup`s.

```c++
class AnimalParams : public tekt::ParamGroup {
public:
  AnimalParams(std::string prefix, std::string page)
  : tekt::ParamGroup(page),
    cuteness(prefix + "cuteness", prefix + " Cuteness", NumericOpts<float>(0.5f, 0.0f, 1.0f)),
    spawnNew(prefix + "spawnpulse", "Spawn New " + prefix)
  {
    // Remember to register each parameter with the group in the constructor.
    add(cuteness);
    add(spawnNew);
  }

  FloatParameter cuteness;
  PulseParameter spawnNew;
};

class ForestSettings : public tekt::Settings {
public:
  ForestSettings()
  : bears("Bear", "Bear Settings"),
    moose("Moose", "Moose Settings")
  {
    // Remember to register each parameter group in the constructor.
    add(bears);
    add(moose);
  }

  AnimalParams bears;
  AnimalParams moose;
};

// Setting up the parameters
void ForestCHOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
  _settings.create(manager);
}

// Handling pulses
void ForestCHOP::pulsePressed(const char* name, void* reserved1)
{
  _settings.handlePulse(name);
}

// Loading and using the values. This could would be done in the getOutputInfo()
// method if the settings will influence the length of the CHOP.
void ForestCHOP::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved) {
  _settings.load(inputs);
  
  if (_settings.bears.spawnNew.getAndReset()) {
    createABear();
  }
  if (_settings.moose.spawnNew.getAndReset()) {
    makeMeAMoose();
  }
  updateTheBears(_settings.bears.cuteness.get());
  modifyTheMoose(_settings.moose.cuteness.get());
}
```
