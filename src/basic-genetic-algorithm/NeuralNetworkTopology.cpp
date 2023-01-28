
#include "NeuralNetworkTopology.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

#include <exception>
#include <stdexcept>

void
NeuralNetworkTopology::init(
  uint32_t input, const HiddenLayers& hiddens, uint32_t output,
  bool useBias /*= true*/) {
  if (input == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of inputs, input=" << input);

  for (uint32_t value : hiddens)
    if (value == 0)
      D_THROW(
        std::invalid_argument,
        "received invalid number of hidden neurons, value=" << value);

  if (output == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of outputs, output=" << output);

  //

  _input = input;
  _output = output;
  _hiddens = hiddens;
  _useBias = useBias;

  //

  _computeTotalWeights();
  _computeTotalNeurons();
}

void
NeuralNetworkTopology::init(
  const std::initializer_list<uint32_t>& list, bool useBias /*= true*/) {
  if (list.size() < 2)
    D_THROW(
      std::invalid_argument,
      "received invalid number of layers, list=" << list.size());

  for (uint32_t value : list)
    if (value == 0)
      D_THROW(
        std::invalid_argument,
        "received invalid number of neurons, value=" << value);

  //

  auto it = list.begin();
  if (list.size() > 2)
    _hiddens.reserve(list.size() - 2);
  for (std::size_t ii = 0; ii < list.size(); ++ii, ++it) {
    if (ii == 0)
      _input = *it;
    else if (ii + 1 == list.size())
      _output = *it;
    else
      _hiddens.push_back(*it);
  }

  _useBias = useBias;

  //

  _computeTotalWeights();
  _computeTotalNeurons();
}

void
NeuralNetworkTopology::_computeTotalWeights() {
  uint32_t prev_layer_num_neuron = _input;
  for (uint32_t num_neuron : _hiddens) {
    _totalWeights += prev_layer_num_neuron * num_neuron;
    prev_layer_num_neuron = num_neuron;
  }
  _totalWeights += prev_layer_num_neuron * _output;
}

void
NeuralNetworkTopology::_computeTotalNeurons() {
  _totalNeurons = _input;
  for (uint32_t size : _hiddens)
    _totalNeurons += size;
  _totalNeurons += _output;
}

bool
NeuralNetworkTopology::isValid() const {
  if (_input == 0 && _output == 0)
    return false;

  for (uint32_t hiddenValue : _hiddens)
    if (hiddenValue == 0)
      return false;

  return true;
}

uint32_t
NeuralNetworkTopology::getInput() const {
  return _input;
}

uint32_t
NeuralNetworkTopology::getOutput() const {
  return _output;
}

const NeuralNetworkTopology::HiddenLayers&
NeuralNetworkTopology::getHiddens() const {
  return _hiddens;
}

bool
NeuralNetworkTopology::isUsingBias() const {
  return _useBias;
}

uint32_t
NeuralNetworkTopology::getTotalWeights() const {
  return _totalWeights;
}

uint32_t
NeuralNetworkTopology::getTotalNeurons() const {
  return _totalNeurons;
}
