
#include "GenomesAncestor.hpp"

#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <algorithm> // std::sort

GenomesAncestor::GenomesAncestor(uint64_t inId)
  : id(inId)
{
  elites.reserve(4);
}

// GenomesAncestor::GenomesAncestor(const GenomesAncestor& other)
// {
//   if (&other == this)
//     return;

//   // D_MYERR("copy ctor");

//   id = other.id;
//   totalReused = other.totalReused;
//   genome = other.genome;
//   elites = other.elites;
// }

// GenomesAncestor::GenomesAncestor(GenomesAncestor&& other)
// {
//   if (&other == this)
//     return;

//   // D_MYERR("move ctor");

//   std::swap(id, other.id);
//   std::swap(totalReused, other.totalReused);
//   genome = std::move(other.genome);
//   elites = std::move(other.elites);
// }

// GenomesAncestor& GenomesAncestor::GenomesAncestor::operator=(const GenomesAncestor& other)
// {
//   if (&other == this)
//     return *this;

//   // D_MYERR("copy op=");

//   id = other.id;
//   totalReused = other.totalReused;
//   genome = other.genome;
//   elites = other.elites;

//   return *this;
// }

// GenomesAncestor& GenomesAncestor::GenomesAncestor::operator=(GenomesAncestor&& other)
// {
//   if (&other == this)
//     return *this;

//   // D_MYERR("move op=");

//   std::swap(id, other.id);
//   std::swap(totalReused, other.totalReused);
//   genome = std::move(other.genome);
//   elites = std::move(other.elites);

//   return *this;
// }

void GenomesAncestor::addNewElite(const Genome& inGenome, float inReusedAncestorScorePenalty)
{
  if (elites.empty())
  {
    totalReused += 1;
    elites.push_back(inGenome);
  }
  else
  {
    const float newFitness = inGenome.fitness;
    const float worstEliteFitness = elites.back().fitness;
    const bool newOneIsFitter = newFitness > worstEliteFitness;

    if (newOneIsFitter)
    {
      elites.push_back(inGenome);

      std::sort(elites.begin(), elites.end(), [](
        const Genome& lhs,
        const Genome& rhs
      ) {
        // higher is better
        return lhs.fitness > rhs.fitness;
      });

      while (elites.size() > 3)
        elites.pop_back();
    }
    else
    {
      totalReused += 1;
    }
  }

  computePriorityScore(inReusedAncestorScorePenalty);
}

void GenomesAncestor::computePriorityScore(float inReusedAncestorScorePenalty)
{
  cachedPriorityScore = getFitness() - float(totalReused) * inReusedAncestorScorePenalty;
}


const Genome& GenomesAncestor::getGenome() const
{
  if (elites.empty())
    return genome;

  if (elites.size() == 1)
    return elites.front();

  const int index = gero::rng::RNG::getRangedValue(0, int(elites.size()));
  return elites.at(std::size_t(index));
}

float GenomesAncestor::getFitness() const
{
  if (elites.empty())
    return genome.fitness;

  return std::max(genome.fitness, elites.front().fitness);
}

float GenomesAncestor::getPriorityScore() const
{
  return cachedPriorityScore;
}

bool GenomesAncestor::operator<(const GenomesAncestor& other) const
{
  return (getPriorityScore() < other.getPriorityScore());
}
