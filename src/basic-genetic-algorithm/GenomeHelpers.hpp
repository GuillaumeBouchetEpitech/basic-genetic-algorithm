
#pragma once

#include "Genome.hpp"

namespace GenomeHelpers {

  void reproduce(
    const Genome& inParentA,
    const Genome& inParentB,
    uint32_t inTotalWeights,
    Genome& outOffspring);

  void mutate(
    Genome& inGenome,
    uint32_t inMinimumMutation = 0);

  float compare(
    const Genome& genomeA,
    const Genome& genomeB,
    uint32_t inTotalWeights);

  void randomiseConnectionWeights(Genome& inGenome, uint32_t inTotalWeights);

}
