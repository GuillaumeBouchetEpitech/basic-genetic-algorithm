
#include "GenomeHelpers.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

namespace GenomeHelpers {

  void
  reproduce(
    const Genome& inParentA,
    const Genome& inParentB,
    uint32_t inTotalWeights,
    Genome& outOffspring
  ) {
    // default of 50/50 chances for both parents
    int32_t chancesForParentA = 50; // 50%

    // 60/40 chances for the fittest parent
    if (inParentA.fitness > inParentB.fitness)
      chancesForParentA = 60; // 60%
    else if (inParentA.fitness < inParentB.fitness)
      chancesForParentA = 40; // 40%

    // crossover

    outOffspring.connectionsWeights.clear();
    outOffspring.connectionsWeights.reserve(inTotalWeights); // pre-allocate

    for (uint32_t ii = 0; ii < inTotalWeights; ++ii) {
      if (gero::rng::RNG::getRangedValue(0, 100) < chancesForParentA)
        outOffspring.connectionsWeights.push_back(inParentA.connectionsWeights.at(ii));
      else
        outOffspring.connectionsWeights.push_back(inParentB.connectionsWeights.at(ii));
    }
  }

  void
  mutate(Genome& inGenome, uint32_t inMinimumMutation /* = 0 */) {
    constexpr int32_t mutationMaxChance = 20; // 20%
    constexpr float mutationMaxEffect = 0.2f; // 20% x 2 = 40%

    uint32_t totalMutation = 0;

    do {

      for (float& weight : inGenome.connectionsWeights)
      {
        if (gero::rng::RNG::getRangedValue(0, 100) < mutationMaxChance)
        {
          weight +=
            gero::rng::RNG::getRangedValue(-mutationMaxEffect, +mutationMaxEffect);

          totalMutation += 1;
        }
      }
    }
    while (totalMutation < inMinimumMutation);
  }

  float compare(
    const Genome& genomeA,
    const Genome& genomeB,
    uint32_t inTotalWeights)
  {

    if (&genomeA == &genomeB) {
      D_THROW(std::runtime_error, "comparing asme genomes");
    }

    const auto& weightsA = genomeA.connectionsWeights;
    const auto& weightsB = genomeB.connectionsWeights;

    if (weightsA.size() != weightsB.size()) {
      D_THROW(std::runtime_error, "comparing genomes with non-matching sizes"
        << ", genomeA connections weights size " << weightsA.size()
        << ", genomeB connections weights size " << weightsB.size()
        );
    }


    float weightDiff = 0.0f;

    for (uint32_t ii = 0; ii < inTotalWeights; ++ii)
    {
      const float currDiff = weightsB.at(ii) - weightsA.at(ii);
      // weightDiff += currDiff;
      weightDiff += (currDiff < 0.0f ? -currDiff : currDiff);
    }

    // D_MYLOG(" -> weightDiff=" << weightDiff << ", inTotalWeights=" << inTotalWeights);

		return weightDiff / float(inTotalWeights);

    // float finalVal = weightDiff / float(inTotalWeights);
		// return (finalVal < 0.0f ? -finalVal : finalVal);

    // float finalVal = weightDiff / float(inTotalWeights);
    // // D_MYLOG(" -> finalVal=" << finalVal << ", weightDiff=" << weightDiff << ", inTotalWeights=" << inTotalWeights);
		// return finalVal;
    // return weightDiff;
  }

  void randomiseConnectionWeights(Genome& inGenome, uint32_t inTotalWeights)
  {
    inGenome.connectionsWeights.clear();
    inGenome.connectionsWeights.resize(inTotalWeights);
    for (float& weight : inGenome.connectionsWeights)
      weight = gero::rng::RNG::getRangedValue(-1.0f, 1.0f);
  }

}
