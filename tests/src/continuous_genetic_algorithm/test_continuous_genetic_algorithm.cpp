

#if 0

#include "headers.hpp"

#define D_SHOW_DEV_LOGS



namespace {

template<std::size_t totalInput, std::size_t totalOutput>
struct TrainingData
{
  std::array<float, totalInput> inputs;
  std::array<float, totalOutput> outputs;
};

template<std::size_t totalInput, std::size_t totalOutput, std::size_t totalData>
using AllTrainingData = std::array<TrainingData<totalInput, totalOutput>, totalData>;

template<std::size_t totalInput, std::size_t totalOutput, std::size_t totalData>
void runTest_continuous(
  const AllTrainingData<totalInput, totalOutput, totalData>& inAllTrainingData,
  const NeuralNetworkTopology& inNeuralNetworkTopology,
  uint32_t maxCompletedGenomes)
{

  //
  //
  //

  ContinuousSpeciatedGeneticAlgorithm::Definition genAlgoDef;
  genAlgoDef.topology = inNeuralNetworkTopology;
  // genAlgoDef.totalGenomes = 300;

  genAlgoDef.initalTotalAncestors = 32;
  genAlgoDef.initalDiversityAttempt = 40;
  genAlgoDef.minimumMutations = 2;

  genAlgoDef.weightCoef = 0.15f;
  // genAlgoDef.weightCoef = 3.0f;
  // genAlgoDef.weightCoef = 0.2f * float(inNeuralNetworkTopology.getTotalWeights());

  genAlgoDef.reusedAncestorScorePenalty = 0.5f;

  //
  //
  //

	ContinuousSpeciatedGeneticAlgorithm genAlgo;
  genAlgo.initialise(genAlgoDef);

	constexpr float k_maximumFitness = float(inAllTrainingData.size());
	constexpr float k_minimumFitness = k_maximumFitness - 0.05f;

  std::vector<float> tmpInputs;
  std::vector<float> tmpOutputs;

  do
  {
    // const std::size_t totalGenomes = genAlgo.getTotalGenomes();

    // for (std::size_t ii = 0; ii < totalGenomes; ++ii)
    // {
    //   AbstractGenome& currGenome = genAlgo.getGenome(ii);
      ContinuousSpeciatedGeneticAlgorithm::GenomeWeakRef currGenomeRef = genAlgo.acquireNewGenome();

      float currentFitness = k_maximumFitness;

      for (const auto& trainingData : inAllTrainingData)
      {
        tmpInputs.clear();
        tmpInputs.reserve(trainingData.inputs.size());
        tmpOutputs.clear();
        tmpOutputs.reserve(trainingData.outputs.size());

        for (float input : trainingData.inputs)
          tmpInputs.push_back(input);

        NeuralNetwork neuralNetwork(inNeuralNetworkTopology);
        neuralNetwork.setConnectionsWeights(currGenomeRef->getConnectionsWeights());
        neuralNetwork.compute(tmpInputs, tmpOutputs);

        const float value = std::min(std::max(tmpOutputs.at(0), 0.0f), 1.0f);

        const float diff = value - trainingData.outputs.at(0);
        currentFitness -= diff * diff;
      }

      // currGenome.rate(currentFitness);

      genAlgo.discardGenome(currGenomeRef, currentFitness);

    // }


    // genAlgo.breedPopulation();

    const AbstractGenome& best = genAlgo.getBestGenome();
    if (best.getFitness() >= k_minimumFitness)
      break;

    ASSERT_LE(genAlgo.getTotalCompletedGenomes(), maxCompletedGenomes)
      << std::endl
      << std::endl
      << " ===========> too slow, best was "
      << std::fixed << std::setprecision(2) << (float(best.getFitness()) / k_minimumFitness) << "%"
      << std::endl
      << std::endl
      << "getTotalCompletedGenomes " << genAlgo.getTotalCompletedGenomes()
      << std::endl
      << "getTotalAncestors " << genAlgo.getTotalAncestors()
      << std::endl
      << "weightCoef " << genAlgoDef.weightCoef
      << std::endl
      << "TotalWeights " << inNeuralNetworkTopology.getTotalWeights()
      << std::endl
      ;

  }
  while (true);


  {
    std::string tmpLog = D_SSTR("##########" << std::endl
      // << "too slow, best was "
      // << std::fixed << std::setprecision(2) << (float(best.getFitness()) / k_minimumFitness) << "%"
      // << std::endl
      << std::endl
      << std::endl
      << "getTotalCompletedGenomes " << genAlgo.getTotalCompletedGenomes()
      << std::endl
      << "getTotalAncestors " << genAlgo.getTotalAncestors()
      << std::endl
      << "weightCoef " << genAlgoDef.weightCoef
      << std::endl
      << "TotalWeights " << inNeuralNetworkTopology.getTotalWeights()
      << std::endl
      << std::endl);

    D_MYERR(tmpLog);

  }

  {
    const AbstractGenome& best = genAlgo.getBestGenome();

    ASSERT_GE(best.getFitness(), k_minimumFitness);

    NeuralNetwork neuralNetwork(inNeuralNetworkTopology);
    neuralNetwork.setConnectionsWeights(best.getConnectionsWeights());

    for (const auto& trainingData : inAllTrainingData)
    {
      tmpInputs.clear();
      tmpOutputs.clear();
      tmpInputs.reserve(trainingData.inputs.size());
      tmpOutputs.reserve(trainingData.outputs.size());

      for (const float input : trainingData.inputs)
        tmpInputs.push_back(input);

      neuralNetwork.compute(tmpInputs, tmpOutputs);

      ASSERT_EQ(tmpInputs.size(), trainingData.inputs.size());
      for (std::size_t ii = 0; ii < trainingData.inputs.size(); ++ii)
        ASSERT_NEAR(tmpInputs.at(ii), trainingData.inputs.at(ii), 0.01f);

      ASSERT_EQ(tmpOutputs.size(), trainingData.outputs.size());
      for (std::size_t ii = 0; ii < trainingData.outputs.size(); ++ii)
        ASSERT_NEAR(tmpOutputs.at(ii), trainingData.outputs.at(ii), 0.25f);

    }

  }
}

}

//
//
//
//
//

constexpr uint32_t k_classicGenerationSize = 10000;

TEST(test_continuous_genetic_algorithm, test_logic_gate___yes) {

  // D_MYERR("TEST <3");

  std::array<TrainingData<1, 1>, 2> allTrainingDatas = {{
    { {{ 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 5 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___no) {

  std::array<TrainingData<1, 1>, 2> allTrainingDatas = {{
    { {{ 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 5 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___and) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___nand) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 2, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___or) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___nor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___xor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 3, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(test_continuous_genetic_algorithm, test_logic_gate___xnor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 3, 1 });

  runTest_continuous(allTrainingDatas, neuralNetworkTopology, 100 * k_classicGenerationSize);
}

#endif
