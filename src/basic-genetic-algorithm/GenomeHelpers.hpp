
#pragma once

#include "Genome.hpp"

#include <functional>

namespace GenomeHelpers {

using GetRandomCallback = std::function<float()>;

void reproduce(
  const Genome& inParentA, const Genome& inParentB, uint32_t inTotalWeights, Genome& outOffspring,
  const GetRandomCallback& randomCallback);

void mutate(
  Genome& inGenome, uint32_t inMinimumMutation, float inMutationMaxChance, float inMutationMaxEffect,
  const GetRandomCallback& randomCallback);

float compare(const Genome& genomeA, const Genome& genomeB, uint32_t inTotalWeights);

void randomizeConnectionWeights(Genome& inGenome, uint32_t inTotalWeights, const GetRandomCallback& randomCallback);

} // namespace GenomeHelpers
