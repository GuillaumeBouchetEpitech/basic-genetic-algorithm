
#pragma once

#include "basic-genetic-algorithm/Genome.hpp"

#include <vector>
#include <cstdint>

// TODO: class + container... factory pattern?
struct GenomesAncestor {

public:
  uint64_t id = 1UL;
  uint32_t totalReused = 0U;

  Genome genome;

  std::vector<Genome> elites;

  float cachedPriorityScore = 0.0f;

public:
  GenomesAncestor(uint64_t inId);
  // GenomesAncestor(const GenomesAncestor& other);
  // GenomesAncestor(GenomesAncestor&& other);
  // GenomesAncestor& operator=(const GenomesAncestor& other);
  // GenomesAncestor& operator=(GenomesAncestor&& other);
  virtual ~GenomesAncestor() = default;

public:
  void addNewElite(const Genome& inGenome, float inReusedAncestorScorePenalty);

  void computePriorityScore(float inReusedAncestorScorePenalty);

public:
  const Genome& getGenome() const;
  float getFitness() const;
  float getPriorityScore() const;
  bool operator<(const GenomesAncestor& other) const;

};
