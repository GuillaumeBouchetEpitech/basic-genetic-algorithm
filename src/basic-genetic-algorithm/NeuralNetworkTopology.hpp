
#pragma once

#include <cstdint>
#include <vector>

class NeuralNetworkTopology {
public:
  using HiddenLayers = std::vector<uint32_t>;

private: // attributes
  uint32_t _input = 0;
  HiddenLayers _hiddens;
  uint32_t _output = 0;

  bool _useBias = false;

  uint32_t _totalWeights = 0;
  uint32_t _totalNeurons = 0;

public:
  NeuralNetworkTopology() = default;

  void init(
    uint32_t input, const HiddenLayers& hiddens, uint32_t output,
    bool useBias = true);

  void init(const std::initializer_list<uint32_t>& list, bool useBias = true);

private:
  void _computeTotalWeights();
  void _computeTotalNeurons();

public:
  bool isValid() const;

public:
  uint32_t getInput() const;
  uint32_t getOutput() const;
  const HiddenLayers& getHiddens() const;

  bool isUsingBias() const;

  uint32_t getTotalWeights() const;
  uint32_t getTotalNeurons() const;
};
