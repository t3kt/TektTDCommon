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
FloatParameter amount = {ParOpts("Amount", "Amount of stuff", 3.2f)};
BoolParameter enable = {ParOpts("Enable", "Enable the stuff", true)};
PulseParameter reset = {ParOpts("Reset", "Reset the stuff"}};

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
