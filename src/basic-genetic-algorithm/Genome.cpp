
#include "Genome.hpp"

uint64_t Genome::_currentId = 1UL;

// default ctor
Genome::Genome() : id(Genome::_currentId++) { id = Genome::_currentId++; }

// copy ctor
Genome::Genome(const Genome& other) {
  if (&other == this)
    return;

  parentIdA = other.parentIdA;
  parentIdB = other.parentIdB;
  id = other.id;
  fitness = other.fitness;
  connectionsWeights = other.connectionsWeights; // reallocation
}

// move ctor
Genome::Genome(Genome&& other) {
  if (&other == this)
    return;

  std::swap(parentIdA, other.parentIdA);
  std::swap(parentIdB, other.parentIdB);
  std::swap(id, other.id);
  std::swap(fitness, other.fitness);
  connectionsWeights = std::move(other.connectionsWeights); // no reallocation
}

// copy operator
Genome&
Genome::operator=(const Genome& other) {
  if (&other == this)
    return *this;

  parentIdA = other.parentIdA;
  parentIdB = other.parentIdB;
  id = other.id;
  fitness = other.fitness;
  connectionsWeights = other.connectionsWeights; // reallocation

  return *this;
}

// move operator
Genome&
Genome::operator=(Genome&& other) {
  if (this == &other)
    return *this;

  std::swap(parentIdA, other.parentIdA);
  std::swap(parentIdB, other.parentIdB);
  std::swap(id, other.id);
  std::swap(fitness, other.fitness);
  connectionsWeights = std::move(other.connectionsWeights); // no reallocation

  return *this;
}

uint64_t
Genome::getId() const {
  return id;
}

float
Genome::getFitness() const {
  return fitness;
}

const std::vector<float>&
Genome::getConnectionsWeights() const {
  return connectionsWeights;
}

void
Genome::rate(float inFitness) {
  fitness = inFitness;
}
