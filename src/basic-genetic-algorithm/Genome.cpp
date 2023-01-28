
#include "Genome.hpp"

uint32_t Genome::_currentId = 1;

// default ctor
Genome::Genome() { id = Genome::_currentId++; }

// copy ctor
Genome::Genome(const Genome& other) {
  id = other.id;
  fitness = other.fitness;
  connectionsWeights = other.connectionsWeights; // reallocation
}

// move ctor
Genome::Genome(Genome&& other) {
  std::swap(id, other.id);
  std::swap(fitness, other.fitness);

  connectionsWeights = std::move(other.connectionsWeights); // no reallocation
}

// copy operator
Genome&
Genome::operator=(const Genome& other) {
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

  std::swap(id, other.id);
  std::swap(fitness, other.fitness);

  connectionsWeights = std::move(other.connectionsWeights); // no reallocation

  return *this;
}

uint32_t
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
