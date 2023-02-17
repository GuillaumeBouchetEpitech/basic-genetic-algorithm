
#pragma once

#include <cstdint>
#include <vector>

class NeuralNetworkTopology {
public:
  using HiddenLayers = std::vector<uint32_t>;

private: // attributes
  uint32_t _inputLayerSize = 0;
  HiddenLayers _hiddenLayers;
  uint32_t _outputLayerSize = 0;

  bool _isUsingBias = false;

  uint32_t _totalWeights = 0;
  uint32_t _totalNeurons = 0;

public:
  NeuralNetworkTopology() = default;
  NeuralNetworkTopology(const NeuralNetworkTopology& other);
  NeuralNetworkTopology(NeuralNetworkTopology&& other);
  NeuralNetworkTopology& operator=(const NeuralNetworkTopology& other);
  NeuralNetworkTopology& operator=(NeuralNetworkTopology&& other);
  virtual ~NeuralNetworkTopology() = default;

  void init(
    uint32_t inputLayerSize, const HiddenLayers& hiddenLayers, uint32_t outputLayerSize,
    bool useBias = true);

  void init(const std::initializer_list<uint32_t>& list, bool useBias = true);

private:
  void _computeTotalWeights();
  void _computeTotalNeurons();

public:
  void validate() const;
  bool isValid() const;

public:
  uint32_t getInputLayerSize() const;
  uint32_t getOutputLayerSize() const;
  const HiddenLayers& getHiddenLayers() const;

  bool isUsingBias() const;

  uint32_t getTotalLayers() const;
  uint32_t getTotalWeights() const;
  uint32_t getTotalNeurons() const;
};
