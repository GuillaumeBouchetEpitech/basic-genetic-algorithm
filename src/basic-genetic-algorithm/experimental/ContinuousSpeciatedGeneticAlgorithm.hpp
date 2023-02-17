
#pragma once

#include "basic-genetic-algorithm/NeuralNetwork.hpp"
#include "basic-genetic-algorithm/Genome.hpp"
#include "basic-genetic-algorithm/GenomeHelpers.hpp"

#include "GenomesAncestor.hpp"

#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/containers/weak_ref_data_pool.hpp"
#include "geronimo/system/containers/dynamic_heap_array.hpp"

#include <array>
#include <cstdint>
#include <set>
#include <functional>


class ContinuousSpeciatedGeneticAlgorithm : public gero::NonCopyable {

public:

  struct Definition {

    uint32_t genomePoolSize = 128;

    NeuralNetworkTopology topology;

    uint32_t initalTotalAncestors = 32;
    uint32_t initalDiversityAttempt = 10;
    uint32_t maxTotalAncestors = 50;

    uint32_t minimumMutations = 0;
    float mutationMaxChance = 0.2f;
    float mutationMaxEffect = 0.2f;
    float ancestorsWeightCoef = 0.2f;
    float reusedAncestorScorePenalty = 0.2f;
    float randomGenomeChance = 0.1f;

    GenomeHelpers::GetRandomCallback getRandomCallback;

    void validate() const;
  };


public:
  using GenomesPool = gero::weak_ref_data_pool<Genome, Genome, 256, false>;
  using GenomeWeakRef = GenomesPool::weak_ref;

public:

  using GenomesAncestors = gero::dynamic_heap_array<GenomesAncestor, GenomesAncestor, 128>;

private: // attributes
  Definition _def;
  GenomesPool _liveGenomesPool;
  GenomesAncestors _genomesAncestors;
  Genome _bestGenome;
  uint32_t _totalCompletedGenomes = 0;
  uint64_t _currentAncestorId = 1UL;

public: // ctor/dtor
  ContinuousSpeciatedGeneticAlgorithm() = default;

public: // methods
  void initialise(const Definition& def);

public: // method(s)

  GenomeWeakRef acquireNewGenome();
  void discardGenome(GenomeWeakRef inGenomeRef, float inFitness);

public: // getter(s)
  const AbstractGenome& getBestGenome() const;
  uint32_t getTotalCompletedGenomes() const;
  uint32_t getTotalAncestors() const;

};
