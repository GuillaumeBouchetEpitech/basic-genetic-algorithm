
#include "NeuralNetworkTopology.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

#include <exception>
#include <stdexcept>

NeuralNetworkTopology::NeuralNetworkTopology(
  const NeuralNetworkTopology& other) {
  if (&other == this)
    return;

  _inputLayerSize = other._inputLayerSize;
  _hiddenLayers = other._hiddenLayers;
  _outputLayerSize = other._outputLayerSize;

  _isUsingBias = other._isUsingBias;

  _totalWeights = other._totalWeights;
  _totalNeurons = other._totalNeurons;
}

NeuralNetworkTopology::NeuralNetworkTopology(NeuralNetworkTopology&& other) {
  if (&other == this)
    return;

  std::swap(_inputLayerSize, other._inputLayerSize);
  _hiddenLayers = std::move(other._hiddenLayers);
  std::swap(_outputLayerSize, other._outputLayerSize);

  std::swap(_isUsingBias, other._isUsingBias);

  std::swap(_totalWeights, other._totalWeights);
  std::swap(_totalNeurons, other._totalNeurons);
}

NeuralNetworkTopology&
NeuralNetworkTopology::operator=(const NeuralNetworkTopology& other) {
  if (&other == this)
    return *this;

  _inputLayerSize = other._inputLayerSize;
  _hiddenLayers = other._hiddenLayers;
  _outputLayerSize = other._outputLayerSize;

  _isUsingBias = other._isUsingBias;

  _totalWeights = other._totalWeights;
  _totalNeurons = other._totalNeurons;

  return *this;
}

NeuralNetworkTopology&
NeuralNetworkTopology::operator=(NeuralNetworkTopology&& other) {
  if (&other == this)
    return *this;

  std::swap(_inputLayerSize, other._inputLayerSize);
  _hiddenLayers = std::move(other._hiddenLayers);
  std::swap(_outputLayerSize, other._outputLayerSize);

  std::swap(_isUsingBias, other._isUsingBias);

  std::swap(_totalWeights, other._totalWeights);
  std::swap(_totalNeurons, other._totalNeurons);

  return *this;
}

void
NeuralNetworkTopology::init(
  uint32_t input, const HiddenLayers& hiddens, uint32_t output,
  bool useBias /*= true*/) {

  //

  _inputLayerSize = input;
  _outputLayerSize = output;
  _hiddenLayers = hiddens;
  _isUsingBias = useBias;

  //

  validate();

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
    _hiddenLayers.reserve(list.size() - 2);
  for (std::size_t ii = 0; ii < list.size(); ++ii, ++it) {
    if (ii == 0)
      _inputLayerSize = *it;
    else if (ii + 1 == list.size())
      _outputLayerSize = *it;
    else
      _hiddenLayers.push_back(*it);
  }

  _isUsingBias = useBias;

  //

  _computeTotalWeights();
  _computeTotalNeurons();
}

void
NeuralNetworkTopology::_computeTotalWeights() {
  uint32_t prev_layer_num_neuron = _inputLayerSize;
  for (uint32_t num_neuron : _hiddenLayers) {
    _totalWeights += prev_layer_num_neuron * num_neuron;
    prev_layer_num_neuron = num_neuron;
  }
  _totalWeights += prev_layer_num_neuron * _outputLayerSize;
}

void
NeuralNetworkTopology::_computeTotalNeurons() {
  _totalNeurons = _inputLayerSize;
  for (uint32_t size : _hiddenLayers)
    _totalNeurons += size;
  _totalNeurons += _outputLayerSize;
}

void
NeuralNetworkTopology::validate() const {
  if (_inputLayerSize == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of inputs, input=" << _inputLayerSize);

  for (uint32_t value : _hiddenLayers)
    if (value == 0)
      D_THROW(
        std::invalid_argument,
        "received invalid number of hidden neurons, value=" << value);

  if (_outputLayerSize == 0)
    D_THROW(
      std::invalid_argument,
      "received invalid number of outputs, output=" << _outputLayerSize);
}

bool
NeuralNetworkTopology::isValid() const {
  if (_inputLayerSize == 0 && _outputLayerSize == 0)
    return false;

  for (uint32_t hiddenValue : _hiddenLayers)
    if (hiddenValue == 0)
      return false;

  return true;
}

uint32_t
NeuralNetworkTopology::getInputLayerSize() const {
  return _inputLayerSize;
}

uint32_t
NeuralNetworkTopology::getOutputLayerSize() const {
  return _outputLayerSize;
}

const NeuralNetworkTopology::HiddenLayers&
NeuralNetworkTopology::getHiddenLayers() const {
  return _hiddenLayers;
}

bool
NeuralNetworkTopology::isUsingBias() const {
  return _isUsingBias;
}

uint32_t
NeuralNetworkTopology::getTotalLayers() const {
  return 2U + uint32_t(_hiddenLayers.size());
}

uint32_t
NeuralNetworkTopology::getTotalWeights() const {
  return _totalWeights;
}

uint32_t
NeuralNetworkTopology::getTotalNeurons() const {
  return _totalNeurons;
}
