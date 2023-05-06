
#include "ContinuousSpeciatedGeneticAlgorithm.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <algorithm> // std::sort

//
//
//
//
//

void ContinuousSpeciatedGeneticAlgorithm::Definition::validate() const
{
  if (genomePoolSize == 0)
    D_THROW(
      std::invalid_argument, "received invalid genome pool size"
                               << ", input=" << genomePoolSize
                               << ", expected > 0");

  if (initalTotalAncestors < 8)
    D_THROW(
      std::invalid_argument, "received invalid inital total of ancestors"
                               << ", input=" << initalTotalAncestors
                               << ", expected >= 8");

  if (!topology.isValid())
    D_THROW(std::invalid_argument, "received invalid topology");

  if (!getRandomCallback)
    D_THROW(std::invalid_argument, "missing random callback");

}

//
//
//
//
//

void ContinuousSpeciatedGeneticAlgorithm::initialize(const Definition& def)
{
  _def = def;

  _def.validate();

  const uint32_t totalWeights = _def.topology.getTotalWeights();

  {
    GenomesAncestor newAncestor(_currentAncestorId++);
    GenomeHelpers::randomizeConnectionWeights(
      newAncestor.genome,
      totalWeights,
      _def.getRandomCallback);

    _genomesAncestors.push_back(newAncestor);
  }

  for (uint32_t ancestorIndex = 1; ancestorIndex < _def.initalTotalAncestors; ++ancestorIndex) {

    Genome currGenome;
    Genome mostDiverseGenome;
    float mostDiverseDiff = 0.0f;

    for (uint32_t attempt = 0; attempt < _def.initalDiversityAttempt; ++attempt) {

      GenomeHelpers::randomizeConnectionWeights(
        currGenome,
        totalWeights,
        _def.getRandomCallback);

      float leastDiverseDiff = 0.0f;
      for (const GenomesAncestor& currAncestor : _genomesAncestors) {

        const float currDiff = GenomeHelpers::compare(currGenome, currAncestor.genome, totalWeights);

        if (leastDiverseDiff > currDiff)
          leastDiverseDiff = currDiff;
      }

      if (mostDiverseDiff > 0.0f && mostDiverseDiff >= leastDiverseDiff)
        continue;

      mostDiverseDiff = leastDiverseDiff;
      mostDiverseGenome = currGenome;
    }

    GenomesAncestor newAncestor(_currentAncestorId++);
    newAncestor.genome = mostDiverseGenome;

    _genomesAncestors.push_back(newAncestor);
  }

}

ContinuousSpeciatedGeneticAlgorithm::GenomeWeakRef ContinuousSpeciatedGeneticAlgorithm::acquireNewGenome()
{
  if (_genomesAncestors.size() < 2)
    D_THROW(std::invalid_argument, "not enough ancestors");

  // generate and get a new genome

  // determine parents
  // - check species graph
  //   - if empty -> generate random
  //   - else ->
  //     - select parent according to fitness and recurrence

  // diversity: chance for a randomly generated genome
  if (_def.getRandomCallback() < _def.randomGenomeChance)
  {
    auto newOffspringRef = _liveGenomesPool.acquire();
    if (!newOffspringRef)
      D_THROW(std::runtime_error, "no more genomes in the pool");

    GenomeHelpers::randomizeConnectionWeights(
      *newOffspringRef,
      _def.topology.getTotalWeights(),
      _def.getRandomCallback);

    return newOffspringRef;
  }

  // sort ancestors by priority


  std::sort(_genomesAncestors.begin(), _genomesAncestors.end(), [](
    const GenomesAncestor& lhs,
    const GenomesAncestor& rhs
  ) {
    // higher is better
    return lhs.getPriorityScore() > rhs.getPriorityScore();
  });

  // trim, last ancestors are the under performing ones
  while (_genomesAncestors.size() > _def.maxTotalAncestors)
    _genomesAncestors.pop_back();

  const GenomesAncestor& parentA = _genomesAncestors.at(0);
  const GenomesAncestor& parentB = _genomesAncestors.at(1);

  auto newOffspringRef = _liveGenomesPool.acquire();
  if (!newOffspringRef)
    D_THROW(std::runtime_error, "no more genomes in the pool");

  newOffspringRef->parentIdA = parentA.genome.id;
  newOffspringRef->parentIdB = parentB.genome.id;

  const Genome& parentGenomeA = parentA.getGenome(_def.getRandomCallback);
  const Genome& parentGenomeB = parentB.getGenome(_def.getRandomCallback);

  GenomeHelpers::reproduce(
    parentGenomeA,
    parentGenomeB,
    _def.topology.getTotalWeights(),
    *newOffspringRef,
    _def.getRandomCallback);

  GenomeHelpers::mutate(
    *newOffspringRef,
    _def.minimumMutations,
    _def.mutationMaxChance,
    _def.mutationMaxEffect,
    _def.getRandomCallback);

  return newOffspringRef;
}

void ContinuousSpeciatedGeneticAlgorithm::discardGenome(GenomeWeakRef inGenomeRef, float inFitness)
{
  if (!inGenomeRef.is_active())
    D_THROW(std::invalid_argument, "ref not active ancestors");

  Genome& currGenome = reinterpret_cast<Genome&>(*inGenomeRef.get());
  currGenome.fitness = inFitness;

  if (_bestGenome.fitness < currGenome.fitness)
    _bestGenome = currGenome;

  const uint32_t totalWeights = _def.topology.getTotalWeights();

  int32_t closestMatchIndex = -1;
  float closestMatchDiff = 1000.0f;

  for (int32_t tmpIndex = 0; tmpIndex < int32_t(_genomesAncestors.size()); ++tmpIndex) {

    const float currDiff = GenomeHelpers::compare(currGenome, _genomesAncestors[tmpIndex].genome, totalWeights);

    if (closestMatchDiff > currDiff)
    {
      closestMatchDiff = currDiff;
      closestMatchIndex = tmpIndex;
    }
  }

  if (closestMatchIndex > 0 && closestMatchDiff < _def.ancestorsWeightCoef)
  {
    // matching existing ancestor

    auto& currAncestor = _genomesAncestors.at(std::size_t(closestMatchIndex));
    currAncestor.addNewElite(currGenome, _def.reusedAncestorScorePenalty);
  }
  else
  {
    // new ancestor
    GenomesAncestor newAncestor(_currentAncestorId++);
    newAncestor.genome = std::move(currGenome);

    _genomesAncestors.push_back(newAncestor);
  }

  _liveGenomesPool.release(inGenomeRef);

  _totalCompletedGenomes += 1;
}

const AbstractGenome&
ContinuousSpeciatedGeneticAlgorithm::getBestGenome() const {
  return _bestGenome;
}

uint32_t ContinuousSpeciatedGeneticAlgorithm::getTotalCompletedGenomes() const
{
  return _totalCompletedGenomes;
}

uint32_t ContinuousSpeciatedGeneticAlgorithm::getTotalAncestors() const
{
  return uint32_t(_genomesAncestors.size());
}


