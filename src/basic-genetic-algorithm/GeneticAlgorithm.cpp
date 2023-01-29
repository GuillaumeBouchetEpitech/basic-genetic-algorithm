
#include "GeneticAlgorithm.hpp"

#include "GenomeHelpers.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <algorithm> // <= std::sort
#include <iomanip>   // <= std::fixed / setprecision

void
GeneticAlgorithm::initialise(const Definition& inDef) {
  _def = inDef;

  if (_def.totalGenomes < 10)
    D_THROW(
      std::invalid_argument, "received invalid number of genomes"
                               << ", input=" << _def.totalGenomes
                               << ", expected >= 10");

  if (!_def.topology.isValid())
    D_THROW(std::invalid_argument, "received invalid topology");

  //

  // set the genomes and their neural network

  _genomes.resize(_def.totalGenomes);

  const uint32_t totalElites =
    std::max(5U, uint32_t(float(_def.totalGenomes) * 0.1f)); // 10%
  _eliteGenomes.resize(totalElites);

  _neuralNetworks.reserve(_def.totalGenomes); // pre-allocate

  gero::rng::RNG::ensureRandomSeed();

  for (auto& currGenome : _genomes) {

    GenomeHelpers::randomiseConnectionWeights(currGenome, _def.topology.getTotalWeights());

    auto newNeuralNet = std::make_shared<NeuralNetwork>(_def.topology);
    newNeuralNet->setConnectionsWeights(currGenome.connectionsWeights);
    _neuralNetworks.push_back(newNeuralNet);
  }
}

bool
GeneticAlgorithm::breedPopulation() {
  if (_genomes.empty())
    D_THROW(std::runtime_error, "not initialised");

  Genomes latestBestGenomes;
  _getBestGenomes(latestBestGenomes);

  const auto& latestBestGenome = latestBestGenomes.front();

  const auto& oldBestGenome = _eliteGenomes.front();
  const bool isSmarterGeneration = (latestBestGenome.fitness > oldBestGenome.fitness);

#if 0

  const bool isStallingGeneration =
    (latestBestGenome.fitness == oldBestGenome.fitness);

  D_MYLOG(
    (isSmarterGeneration ? "++" : (isStallingGeneration ? "==" : "--"))
    << " generation=" << _currentGeneration << std::fixed
    << std::setprecision(1) << ", fitness:"
    << " [best=" << oldBestGenome.fitness << "]"
    << " [latest=" << latestBestGenome.fitness << "]"
    << ", diff=" << (isSmarterGeneration ? "+" : "")
    << (latestBestGenome.fitness - oldBestGenome.fitness));

#endif

  { // refresh the elite genomes internal array

    //
    //
    // acumulate the elite genomes + the latest best genomes

    Genomes tmpBestGenomes;
    tmpBestGenomes.reserve(_eliteGenomes.size() + latestBestGenomes.size());
    for (std::size_t ii = 0; ii < _eliteGenomes.size(); ++ii)
      tmpBestGenomes.push_back(_eliteGenomes.at(ii));
    for (std::size_t ii = 0; ii < latestBestGenomes.size(); ++ii)
      tmpBestGenomes.push_back(latestBestGenomes.at(ii));

    //
    //
    // sort the container, from the best to the worst

    // sort by fitness
    auto cmpFunc = [](const Genome& a, const Genome& b) {
      // the higher the better
      return a.fitness > b.fitness;
    };
    std::sort(tmpBestGenomes.begin(), tmpBestGenomes.end(), cmpFunc);

    //
    //
    // replace the elites with the sorted best genomes

    for (std::size_t ii = 0; ii < _eliteGenomes.size(); ++ii)
      _eliteGenomes.at(ii) = tmpBestGenomes.at(ii);

  } // refresh the elite genomes internal array

  Genomes offsprings;
  offsprings.reserve(_genomes.size()); // pre-allocate

  { // elitism: keep the current best

    for (auto& bestGenome : _eliteGenomes)
      if (bestGenome.fitness > 0.0f) {
        // copy, realloc of the weights
        offsprings.push_back(bestGenome);
      }

  } // elitism: keep the current best

  { // crossover: breed best genomes

    struct ParentPair {
      std::size_t parentA;
      std::size_t parentB;
    };

    std::vector<ParentPair> parentsPairsGenomes;

    // build all the possible "parents" pairs
    for (std::size_t ii = 0; ii < latestBestGenomes.size(); ++ii)
      for (std::size_t jj = ii + 1; jj < latestBestGenomes.size(); ++jj)
        parentsPairsGenomes.push_back({ii, jj});

    // sort the possible "parents" pair by summed fitness
    auto cmpFunc =
      [&latestBestGenomes](const ParentPair& a, const ParentPair& b) {
        float fitnessPairA =
          (latestBestGenomes.at(a.parentA).fitness +
           latestBestGenomes.at(a.parentB).fitness);
        float fitnessPairB =
          (latestBestGenomes.at(b.parentA).fitness +
           latestBestGenomes.at(b.parentB).fitness);
        return (fitnessPairA > fitnessPairB); // <= the higher the better
      };
    std::sort(parentsPairsGenomes.begin(), parentsPairsGenomes.end(), cmpFunc);

    int32_t totalOffspringLeft = int32_t(float(_genomes.size()) * 0.9f);
    for (const auto& parentPair : parentsPairsGenomes) {
      // stop here if the offsprings container is already full
      if (offsprings.size() >= _genomes.size())
        break;

      // stop here if the max amount of children is reached
      if (totalOffspringLeft-- <= 0)
        break;

      const auto& parentGenomeA = latestBestGenomes.at(parentPair.parentA);
      const auto& parentGenomeB = latestBestGenomes.at(parentPair.parentB);

      Genome newOffspring;

      GenomeHelpers::reproduce(parentGenomeA, parentGenomeB, _def.topology.getTotalWeights(), newOffspring);
      GenomeHelpers::mutate(newOffspring, _def.minimumMutations);

      // move, no realloc of the weights
      // offsprings.push_back(std::move(newOffspring));
      offsprings.push_back(newOffspring);
    }

  } // crossover: breed best genomes

  { // diversity: add random genomes

    // if there is any space left: add some random genome.
    std::size_t remainingOffsprings = _genomes.size() - offsprings.size();

    const uint32_t totalWeights = _def.topology.getTotalWeights();

    for (std::size_t ii = 0; ii < remainingOffsprings; ++ii) {
      Genome newGenome;

      newGenome.connectionsWeights.reserve(totalWeights); // pre-allocate
      for (uint32_t jj = 0; jj < totalWeights; ++jj)
        newGenome.connectionsWeights.push_back(
          gero::rng::RNG::getRangedValue(-1.0f, 1.0f));

      // move, no realloc of the weights
      offsprings.push_back(std::move(newGenome));
    }

  } // diversity: add random genomes

  _genomes = std::move(offsprings); // move, no realloc of the vector content

  for (std::size_t ii = 0; ii < _genomes.size(); ++ii)
    _neuralNetworks.at(ii)->setConnectionsWeights(_genomes.at(ii).connectionsWeights);

  ++_currentGeneration;

  return isSmarterGeneration;
}

void
GeneticAlgorithm::_getBestGenomes(Genomes& outGenomes) const {
  outGenomes.clear();

  struct SortPair {
    float fitness;
    std::size_t index;
  };
  std::vector<SortPair> sortedGenomes;

  sortedGenomes.reserve(_genomes.size()); // pre-allocate
  for (std::size_t ii = 0; ii < _genomes.size(); ++ii)
    sortedGenomes.push_back({_genomes.at(ii).fitness, ii});

  // sort by fitness
  auto cmpFunc = [](const SortPair& a, const SortPair& b) {
    // the higher the better
    return a.fitness > b.fitness;
  };
  std::sort(sortedGenomes.begin(), sortedGenomes.end(), cmpFunc);

  outGenomes.reserve(_genomes.size()); // pre-allocate
  for (const auto& sortedGenome : sortedGenomes)
    outGenomes.push_back(_genomes.at(sortedGenome.index));
}

//

const NeuralNetworks&
GeneticAlgorithm::getNeuralNetworks() const {
  return _neuralNetworks;
}

std::size_t
GeneticAlgorithm::getTotalGenomes() const {
  return _genomes.size();
}
AbstractGenome&
GeneticAlgorithm::getGenome(std::size_t inIndex) {
  return _genomes.at(inIndex);
}

const AbstractGenome&
GeneticAlgorithm::getGenome(std::size_t inIndex) const {
  return _genomes.at(inIndex);
}

const AbstractGenome&
GeneticAlgorithm::getBestGenome() const {
  return _eliteGenomes.at(0);
}

uint32_t
GeneticAlgorithm::getGenerationNumber() const {
  return _currentGeneration;
}

//

void
GeneticAlgorithm::rateGenome(std::size_t inIndex, float inFitness) {
  if (_genomes.empty())
    D_THROW(std::runtime_error, "not initialised");

  _genomes.at(inIndex).rate(inFitness);
}
