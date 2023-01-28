
#pragma once

#include "NeuralNetwork.hpp"

#include "Genome.hpp"

#include "geronimo/system/NonCopyable.hpp"

#include <array>
#include <cstdint>

class GeneticAlgorithm : public gero::NonCopyable {
public:
  struct Definition {
    uint32_t totalGenomes = 0;
    NeuralNetworkTopology topology;
	  uint32_t minimumMutations = 0;
  };

private: // attributes
  Definition _def;

  Genomes _genomes;
  Genomes _eliteGenomes; // keep X elites

  uint32_t _currentGeneration = 1; // generation number

  NeuralNetworkTopology _neuralNetworkTopology;

  NeuralNetworks _neuralNetworks;

public: // ctor/dtor
  GeneticAlgorithm() = default;

public: // methods
  void initialise(const Definition& def);

public: // method(s)
  bool breedPopulation();

private: // method(s)
  void _getBestGenomes(Genomes& output) const;
  void _reproduce(
    const Genome& inParentA, const Genome& inParentB, Genome& outOffspring) const;
  void _mutate(Genome& inGenome, uint32_t inMinimumMutation = 0) const;

public: // getter(s)
  const NeuralNetworks& getNeuralNetworks() const;
  std::size_t getTotalGenomes() const;
  AbstractGenome& getGenome(std::size_t inIndex);
  const AbstractGenome& getBestGenome() const;
  uint32_t getGenerationNumber() const;

public: // setter(s)
  void rateGenome(std::size_t index, float fitness);
};
