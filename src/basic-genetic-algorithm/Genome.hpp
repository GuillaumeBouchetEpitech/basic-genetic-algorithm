
#pragma once

#include <cstdint>
#include <vector>

class AbstractGenome {
public:
  AbstractGenome() = default;
  virtual ~AbstractGenome() = default;

public:
  virtual uint64_t getId() const = 0;
  virtual float getFitness() const = 0;
  virtual const std::vector<float>& getConnectionsWeights() const = 0;

public:
  virtual void rate(float inFitness) = 0;
};

class Genome : public AbstractGenome {
private:
  static uint64_t _currentId;

public:
  Genome();
  Genome(const Genome& other);
  Genome(Genome&& other);
  Genome& operator=(const Genome& other);
  Genome& operator=(Genome&& other);
  virtual ~Genome() = default;

public:
  uint64_t parentIdA = 0UL;
  uint64_t parentIdB = 0UL;
  uint64_t id = 0UL;
  float fitness = 0.0f;
  std::vector<float> connectionsWeights;

public:
  virtual uint64_t getId() const override;
  virtual float getFitness() const override;
  virtual const std::vector<float>& getConnectionsWeights() const override;

public:
  virtual void rate(float inFitness) override;
};

// using Genomes = std::vector<Genome>;
