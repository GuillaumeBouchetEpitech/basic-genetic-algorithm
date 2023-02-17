
#pragma once

#include "NeuralNetwork.hpp"

#include "Genome.hpp"
#include "GenomeHelpers.hpp"

#include "geronimo/system/NonCopyable.hpp"

#include <array>
#include <cstdint>

class GeneticAlgorithm : public gero::NonCopyable {
public:
  struct Definition {
    uint32_t totalGenomes = 0;
    NeuralNetworkTopology topology;
	  uint32_t minimumMutations = 0;
    float mutationMaxChance = 0.2f;
    float mutationMaxEffect = 0.2f;
    GenomeHelpers::GetRandomCallback getRandomCallback;
  };

private:
  using Genomes = std::vector<Genome>;

private: // attributes
  Definition _def;

  Genomes _genomes;
  Genomes _eliteGenomes; // keep X elites

  uint32_t _currentGeneration = 1; // generation number

public: // ctor/dtor
  GeneticAlgorithm() = default;

public: // methods
  void initialise(const Definition& def);

public: // method(s)
  bool breedPopulation();

public: // method(s)
  void setNeuralNetwork(
    const AbstractGenome& inGenome,
    NeuralNetwork& inNeuralNetwork);

private: // method(s)
  void _getBestGenomes(Genomes& outGenomes) const;

public: // getter(s)
  std::size_t getTotalGenomes() const;
  AbstractGenome& getGenome(std::size_t inIndex);
  const AbstractGenome& getGenome(std::size_t inIndex) const;
  const AbstractGenome& getBestGenome() const;
  uint32_t getGenerationNumber() const;

public: // setter(s)
  void rateGenome(std::size_t inIndex, float inFitness);
};
