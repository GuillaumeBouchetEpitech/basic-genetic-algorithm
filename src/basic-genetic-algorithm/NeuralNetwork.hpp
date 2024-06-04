
#pragma once

#include "NeuralNetworkTopology.hpp"

#include "geronimo/system/NonCopyable.hpp"

#include <memory>
#include <vector>

class NeuralNetwork : public gero::NonCopyable {
private: // internal structures
  struct Neuron {
    float value;
    std::size_t startNeuron;
    std::size_t startWeight;
    std::size_t layerSize;
  };

private: // attributes
  NeuralNetworkTopology _topology;
  mutable std::vector<Neuron> _neurons;
  std::vector<float> _connectionsWeights;

public: // ctor/dtor
  NeuralNetwork(const NeuralNetworkTopology& inTopology);
  NeuralNetwork(const NeuralNetwork& other);
  NeuralNetwork(NeuralNetwork&& other);
  NeuralNetwork& operator=(const NeuralNetwork& other);
  NeuralNetwork& operator=(NeuralNetwork&& other);
  virtual ~NeuralNetwork() = default;

public: // methods
  void compute(const std::vector<float>& inputValues, std::vector<float>& outputValues) const;

public: // setter
  void setConnectionsWeights(const std::vector<float>& inWeights);
  void setConnectionsWeights(const float* inWeightsPtr, uint32_t inWeightsSize);

public: // getter
  void getConnectionsWeights(std::vector<float>& outWeights) const;
  const NeuralNetworkTopology& getTopology() const;

  void getNeuronsValues(std::vector<float>& outNeuronsOutputValues) const;
  void setNeuronsValues(const std::vector<float>& inNeuronsValues);
};

using NeuralNetworks = std::vector<std::shared_ptr<NeuralNetwork>>;
