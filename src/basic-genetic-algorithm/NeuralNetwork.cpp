
#include "NeuralNetwork.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <cmath>
#include <cstdint>
#include <stdexcept>

// #define D_USE_SIGMOID

namespace activations {

#ifdef D_USE_SIGMOID

/**
 * it's a steeper sigmoid
 * => input:  [-x..x]
 * => output: [0..1]
 *
 * Notes:
 * => use "desmos.com" to visualise the curve
 * => link: https://www.desmos.com/calculator
 */
float
steeperSigmoid(float x) {
  return 1.0f / (1.0f + expf(-4.9f * x));
}

#else

/**
 * it's a simple RELU function
 * => input:  [-x..x]
 * => output: [0..x]
 *
 * Notes:
 * => faster learning curve than the sigmoid
 * => link:
 * https://stats.stackexchange.com/questions/126238/what-are-the-advantages-of-relu-over-sigmoid-function-in-deep-neural-networks
 */
float
rectifiedLinearUnit(float x) {
  return std::max(0.0f, x);
}

// /**
//  * classic gero::math::clamp function
//  * => input:  [-value..value]
//  * => output: [minVal..maxVal]
//  */
// float
// clamp(float value, float minVal, float maxVal) {
//   return std::min(std::max(value, minVal), maxVal);
// }

/**
 * it's a custom RELU function
 * => input:  [-x..x]
 * => output: [0..1]
 */
float
customRectifiedLinearUnit(float x) {
  return gero::math::clamp(x, 0.0f, 1.0f);
}

#endif

}; // namespace activations

NeuralNetwork::NeuralNetwork(const NeuralNetworkTopology& inTopology) : _topology(inTopology) {
  // defensive check
  if (!_topology.isValid())
    D_THROW(std::invalid_argument, "invalid neural network topology");

  //
  //

  _connectionsWeights.resize(_topology.getTotalWeights(), 0.0f);

  //
  //

  _neurons.reserve(_topology.getTotalNeurons());

  //
  //

  for (uint32_t index = 0; index < _topology.getInputLayerSize(); ++index)
    _neurons.push_back({0.0f, 0, 0, _topology.getInputLayerSize()});

  //
  //

  std::size_t weightIndex = 0;

  struct LayerData {
    uint32_t start;
    uint32_t size;
  };

  LayerData prevLayer = {0, _topology.getInputLayerSize()};

  const auto& hiddenLayers = _topology.getHiddenLayers();
  for (std::size_t layerIndex = 0; layerIndex < hiddenLayers.size(); ++layerIndex) {

    LayerData currLayer = {prevLayer.size, hiddenLayers.at(layerIndex)};

    //

    for (uint32_t neuronIndex = 0; neuronIndex < currLayer.size; ++neuronIndex) {
      _neurons.push_back({0.0f, prevLayer.start, weightIndex, prevLayer.size});
      weightIndex += prevLayer.size;
    }

    //

    prevLayer.start += prevLayer.size;
    prevLayer.size = currLayer.size;
  }

  //
  //

  for (uint32_t neuronIndex = 0; neuronIndex < _topology.getOutputLayerSize(); ++neuronIndex) {
    _neurons.push_back({0.0f, prevLayer.start, weightIndex, prevLayer.size});
    weightIndex += prevLayer.size;
  }
}

NeuralNetwork::NeuralNetwork(const NeuralNetwork& other) {
  if (&other == this)
    return;

  _topology = other._topology;
  _neurons = other._neurons;
  _connectionsWeights = other._connectionsWeights;
}

NeuralNetwork::NeuralNetwork(NeuralNetwork&& other) {
  if (&other == this)
    return;

  _topology = std::move(other._topology);
  _neurons = std::move(other._neurons);
  _connectionsWeights = std::move(other._connectionsWeights);
}

NeuralNetwork&
NeuralNetwork::operator=(const NeuralNetwork& other) {
  if (&other == this)
    return *this;

  _topology = other._topology;
  _neurons = other._neurons;
  _connectionsWeights = other._connectionsWeights;

  return *this;
}

NeuralNetwork&
NeuralNetwork::operator=(NeuralNetwork&& other) {
  if (&other == this)
    return *this;

  _topology = std::move(other._topology);
  _neurons = std::move(other._neurons);
  _connectionsWeights = std::move(other._connectionsWeights);

  return *this;
}

void
NeuralNetwork::compute(const std::vector<float>& inInputValues, std::vector<float>& outOutputValues) const {

  if (inInputValues.size() != _topology.getInputLayerSize())
    D_THROW(
      std::invalid_argument, "invalid number of input"
                               << ", input=" << inInputValues.size() << ", expected=" << _topology.getInputLayerSize());

  //
  //
  // update input layer

  for (std::size_t ii = 0; ii < inInputValues.size(); ++ii)
    _neurons.at(ii).value = inInputValues.at(ii);

  //
  //
  // process all layers

  const std::size_t totalNeurons = _neurons.size();
  static_cast<void>(totalNeurons); // unused

  for (std::size_t currNeuronIndex = _topology.getInputLayerSize(); currNeuronIndex < _neurons.size();
       ++currNeuronIndex) {
    auto& currNeuron = _neurons.at(currNeuronIndex);

    // Sum the weights to the activation value.
    float summedValues = 0.0f;

    const std::size_t totalWeight = _connectionsWeights.size();
    static_cast<void>(totalWeight); // unused

    for (std::size_t prevNeuronIndex = 0; prevNeuronIndex < currNeuron.layerSize; ++prevNeuronIndex) {
      const float currValue = _neurons.at(currNeuron.startNeuron + prevNeuronIndex).value;
      const float currWeight = _connectionsWeights.at(currNeuron.startWeight + prevNeuronIndex);

      summedValues += currValue * currWeight;
    }

    if (_topology.isUsingBias())
      summedValues += 1.0f;

#ifdef D_USE_SIGMOID
    // slower learning speed
    currNeuron.value = activations::steeperSigmoid(summedValues);
#else
    // faster learning speed
    // currNeuron.value = activations::rectifiedLinearUnit(summedValues);
    currNeuron.value = activations::customRectifiedLinearUnit(summedValues);
#endif
  }

  const std::size_t stopIndex = _topology.getTotalNeurons();
  const std::size_t startIndex = stopIndex - _topology.getOutputLayerSize();

  for (std::size_t ii = startIndex; ii < stopIndex; ++ii)
    outOutputValues.push_back(_neurons.at(ii).value);
}

void
NeuralNetwork::setConnectionsWeights(const std::vector<float>& inWeights) {
  setConnectionsWeights(inWeights.data(), uint32_t(inWeights.size()));
}

void
NeuralNetwork::setConnectionsWeights(const float* inWeightsPtr, uint32_t inWeightsSize) {
  const uint32_t totalWeights = _topology.getTotalWeights();

  // defensive check
  if (inWeightsSize != totalWeights) {
    D_THROW(
      std::invalid_argument, "received invalid number of weights"
                               << ", expected=" << totalWeights << ", input=" << inWeightsSize);
  }

  for (std::size_t ii = 0; ii < _connectionsWeights.size(); ++ii)
    _connectionsWeights.at(ii) = inWeightsPtr[ii];
}

void
NeuralNetwork::getConnectionsWeights(std::vector<float>& outWeights) const {
  outWeights.clear();
  outWeights.reserve(_connectionsWeights.size()); // pre-allocate

  for (const float weight : _connectionsWeights)
    outWeights.push_back(weight);
}

const NeuralNetworkTopology&
NeuralNetwork::getTopology() const {
  return _topology;
}

void
NeuralNetwork::getNeuronsValues(std::vector<float>& outNeuronsOutputValues) const {
  outNeuronsOutputValues.clear();
  outNeuronsOutputValues.reserve(_topology.getTotalNeurons());

  for (const auto& neuron : _neurons)
    outNeuronsOutputValues.push_back(neuron.value);
}

void
NeuralNetwork::setNeuronsValues(const std::vector<float>& inNeuronsValues) {
  const uint32_t totalNeurons = _topology.getTotalNeurons();

  // defensive check
  if (inNeuronsValues.size() != totalNeurons)
    D_THROW(
      std::invalid_argument, "received invalid number of neurons"
                               << ", expected=" << totalNeurons << ", input=" << inNeuronsValues.size());

  for (std::size_t ii = 0; ii < _neurons.size(); ++ii)
    _neurons.at(ii).value = inNeuronsValues.at(ii);
}
