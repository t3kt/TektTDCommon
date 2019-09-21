#include "TDChannels.h"

using namespace tekt;

void ChannelMap::getChannelName(int32_t index, OP_String* name) const {
  if (index < 0 || index >= channelCount()) {
    name->setString("INVALID");
  } else {
    name->setString(channelName(index).c_str());
  }
}


ChannelMap& ChannelMap::add(const std::string& name)
{
  _indicesByName[name] = static_cast<int32_t>(_orderedNames.size());
  _orderedNames.push_back(name);
  return *this;
}

ChannelMap& ChannelMap::addIfMissing(const std::string& name)
{
  if (_indicesByName.find(name) == _indicesByName.end())
  {
    add(name);
  }
  return *this;
}

ChannelMap& ChannelMap::addFromInput(const OP_CHOPInput* input)
{
  for (auto i = 0; i < input->numChannels; i++)
  {
    std::string name = input->getChannelName(i);
    add(name);
    _unusedInputNames.insert(name);
  }
  return *this;
}

void ChannelMap::clear()
{
  _indicesByName.clear();
  _orderedNames.clear();
  _unusedInputNames.clear();
}

const float* ChannelMap::inputData(const OP_CHOPInput* input, const std::string& name)
{
  if (input == nullptr) return nullptr;
  auto i = channelIndex(name);
  if (i == -1) return nullptr;
  _unusedInputNames.erase(name);
  return input->getChannelData(i);
}

float* ChannelMap::outputData(CHOP_Output* output, const std::string& name) const
{
  if (output == nullptr) return nullptr;
  auto i = channelIndex(name);
  if (i == -1) return nullptr;
  return output->channels[i];
}

int32_t ChannelMap::channelIndex(const std::string& name) const
{
  auto iter = _indicesByName.find(name);
  if (iter == _indicesByName.end()) return -1;
  return iter->second;
}
