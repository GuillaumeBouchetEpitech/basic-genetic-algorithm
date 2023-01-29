
#include "ContinuousSpeciatedGeneticAlgorithm.hpp"

#include "basic-genetic-algorithm/GenomeHelpers.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <algorithm> // std::sort
// #include <set>

//
//
//
//
//

void ContinuousSpeciatedGeneticAlgorithm::initialise(const Definition& def)
{
  _def = def;

  if (_def.initalTotalAncestors < 8)
    D_THROW(
      std::invalid_argument, "received invalid inital total of ancestors"
                               << ", input=" << _def.initalTotalAncestors
                               << ", expected >= 8");

  if (!_def.topology.isValid())
    D_THROW(std::invalid_argument, "received invalid topology");

  //
  //
  //
  //
  //

  const uint32_t totalWeights = _def.topology.getTotalWeights();

  {
    GenomesAncestor newAncestor(_currentAncestorId++);
    GenomeHelpers::randomiseConnectionWeights(newAncestor.genome, totalWeights);

#if 1
    _genomesAncestors.push_back(newAncestor);
#else
    _genomesAncestors.insert(newAncestor);
#endif
  }

  for (uint32_t ancestorIndex = 1; ancestorIndex < _def.initalTotalAncestors; ++ancestorIndex) {

    Genome currGenome;
    Genome mostDiverseGenome;
    float mostDiverseDiff = 0.0f;

    for (uint32_t attempt = 0; attempt < _def.initalDiversityAttempt; ++attempt) {

      GenomeHelpers::randomiseConnectionWeights(currGenome, totalWeights);

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

#if 1
    _genomesAncestors.push_back(newAncestor);
#else
    _genomesAncestors.insert(newAncestor);
#endif
  }

}

ContinuousSpeciatedGeneticAlgorithm::GenomeWeakRef ContinuousSpeciatedGeneticAlgorithm::acquireNewGenome()
{
  if (_genomesAncestors.size() < 2)
    // return GenomeWeakRef::make_invalid();
    D_THROW(std::invalid_argument, "not enough ancestors");

  // D_MYERR("step");

  // generate and get a new genome


  // determine parents
  // - check species graph
  //   - if empty -> generate random
  //   - else ->
  //     - select parent according to fitness and reccurence

#if 1


  if (gero::rng::RNG::getRangedValue(0, 100) < 5)
  {
    auto newOffspringRef = _liveGenomesPool.acquire();
    if (!newOffspringRef)
      D_THROW(std::runtime_error, "no more genomes in the pool");

    GenomeHelpers::randomiseConnectionWeights(*newOffspringRef, _def.topology.getTotalWeights());
    return newOffspringRef;
  }


  if (_timeSinceLastDefrag == 0)
  {

    std::sort(_genomesAncestors.begin(), _genomesAncestors.end(), [](
      const GenomesAncestor& lhs,
      const GenomesAncestor& rhs
    ) {
      // // higher is better
      // return lhs.getPriorityScore() > rhs.getPriorityScore();

      // I WAS HERE
      // I WAS HERE
      // I WAS HERE
      // I WAS HERE
      // I WAS HERE
      // I WAS HERE
      // I WAS HERE

      // lower is better
      return lhs.getPriorityScore() < rhs.getPriorityScore();
    });

    _timeSinceLastDefrag = 50;

    // // set as last (highest score)
    // _startIndexSinceLastDefrag = int32_t(_genomesAncestors.size()) - 1;
  }

  GenomesAncestor* pParentA = nullptr;
  GenomesAncestor* pParentB = nullptr;

  // int32_t currIndex = _startIndexSinceLastDefrag;
  int32_t currIndex = int32_t(_genomesAncestors.size()) - 1;

  {
    auto& completeLastOne = _genomesAncestors.at(std::size_t(currIndex));
    currIndex -= 1;
    auto& secondLastOne = _genomesAncestors.at(std::size_t(currIndex));
    currIndex -= 1;

    if (completeLastOne.getPriorityScore() >= secondLastOne.getPriorityScore())
    {
      pParentA = &completeLastOne;
      pParentB = &secondLastOne;
    }
    else
    {
      pParentA = &secondLastOne;
      pParentB = &completeLastOne;
    }

  }

  // for (std::size_t ii = 2; ii < _genomesAncestors.size(); ++ii) {

  while (currIndex >= 0) {

    GenomesAncestor& currAncestor = _genomesAncestors.at(std::size_t(currIndex));

    if (currAncestor.getPriorityScore() > pParentA->getPriorityScore())
    {
      pParentB = pParentA;
      pParentA = &currAncestor;
    }
    else if (currAncestor.getPriorityScore() > pParentB->getPriorityScore())
    {
      pParentB = &currAncestor;
    }

    --currIndex;
  }

  const GenomesAncestor& parentA = *pParentA;
  const GenomesAncestor& parentB = *pParentB;

#else

#if 1
  // sort ancestors by priority

  std::sort(_genomesAncestors.begin(), _genomesAncestors.end(), [](
    const GenomesAncestor& lhs,
    const GenomesAncestor& rhs
  ) {
    // higher is better
    return lhs.getPriorityScore() > rhs.getPriorityScore();
  });


#endif

#if 1
  const GenomesAncestor& parentA = _genomesAncestors.at(0);
  const GenomesAncestor& parentB = _genomesAncestors.at(1);
#else
  auto itSet = _genomesAncestors.begin();
  const GenomesAncestor& parentA = *itSet;
  const GenomesAncestor& parentB = *(++itSet);
#endif

#endif

  auto newOffspringRef = _liveGenomesPool.acquire();
  if (!newOffspringRef)
    D_THROW(std::runtime_error, "no more genomes in the pool");

  newOffspringRef->parentIdA = parentA.genome.id;
  newOffspringRef->parentIdB = parentB.genome.id;

  const Genome& parentGenomeA = parentA.getGenome();
  const Genome& parentGenomeB = parentB.getGenome();

  GenomeHelpers::reproduce(parentGenomeA, parentGenomeB, _def.topology.getTotalWeights(), *newOffspringRef);
  GenomeHelpers::mutate(*newOffspringRef, _def.minimumMutations);

  return newOffspringRef;
}

void ContinuousSpeciatedGeneticAlgorithm::discardGenome(GenomeWeakRef inGenomeRef, float inFitness)
{
  if (!inGenomeRef.is_active())
    D_THROW(std::invalid_argument, "ref not active ancestors");

  // D_MYERR("step");

  Genome& currGenome = reinterpret_cast<Genome&>(*inGenomeRef.get());
  currGenome.fitness = inFitness;

  if (_bestGenome.fitness < currGenome.fitness)
    _bestGenome = currGenome;

  const uint32_t totalWeights = _def.topology.getTotalWeights();

  // GenomesAncestor* closestMatchAncestor = nullptr;
  // ContinuousSpeciatedGeneticAlgorithm::GenomesAncestors::iterator closestMatchAncestorid = _genomesAncestors.end();
  auto closestMatchAncestor = _genomesAncestors.end();
  float closestMatchDiff = 1000.0f;

  auto it = _genomesAncestors.begin();
  for (; it != _genomesAncestors.end(); ++it) {

    const float currDiff = GenomeHelpers::compare(currGenome, it->genome, totalWeights);

    if (closestMatchDiff > currDiff)
    {
      closestMatchDiff = currDiff;
      closestMatchAncestor = it;
    }
  }


  // D_MYERR("closestMatchAncestor " << closestMatchAncestor << ", closestMatchDiff " << closestMatchDiff);


  // if (closestMatchAncestor != nullptr && closestMatchDiff < _def.weightCoef)
  if (closestMatchAncestor != _genomesAncestors.end() && closestMatchDiff < _def.weightCoef)
  {
    // macthing existing ancestor


#if 1

    closestMatchAncestor->addNewElite(currGenome, _def.reusedAncestorScorePenalty);

#else

    // GenomesAncestor tmpAncestor = std::move(*it);
    GenomesAncestor tmpCopy = *it;
    tmpCopy.totalReused += 1;

    _genomesAncestors.erase(it);
    _genomesAncestors.insert(tmpCopy);

#endif


  }
  else
  {
    // new ancestor
    GenomesAncestor newAncestor(_currentAncestorId++);
    newAncestor.genome = std::move(currGenome); // TODO: move it instead?

#if 1
    _genomesAncestors.push_back(newAncestor);
#else
    _genomesAncestors.insert(newAncestor);
#endif
  }

  _liveGenomesPool.release(inGenomeRef);

  _totalCompletedGenomes += 1;

  if (_timeSinceLastDefrag > 0)
    _timeSinceLastDefrag -= 1;
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
  return uint32_t(_genomesAncestors.size()) - _def.initalTotalAncestors;
}


