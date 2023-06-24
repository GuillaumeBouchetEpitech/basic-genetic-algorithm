

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
  std::string_view inTestName,
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
  genAlgoDef.maxTotalAncestors = 300;
  genAlgoDef.minimumMutations = 0;
  genAlgoDef.mutationMaxChance = 0.2f;
  genAlgoDef.mutationMaxEffect = 0.2f;

  // genAlgoDef.ancestorsWeightCoef = 0.10f;
  genAlgoDef.ancestorsWeightCoef = 0.15f;
  // genAlgoDef.ancestorsWeightCoef = 0.20f;
  // genAlgoDef.ancestorsWeightCoef = 0.3f;
  // genAlgoDef.ancestorsWeightCoef = 0.5f;
  // genAlgoDef.ancestorsWeightCoef = 3.0f;
  // genAlgoDef.ancestorsWeightCoef = 0.2f * float(inNeuralNetworkTopology.getTotalWeights());

  // genAlgoDef.reusedAncestorScorePenalty = 0.5f;
  genAlgoDef.reusedAncestorScorePenalty = 0.3f;
  // genAlgoDef.reusedAncestorScorePenalty = 0.2f;
  // genAlgoDef.reusedAncestorScorePenalty = 0.1f;
  // genAlgoDef.reusedAncestorScorePenalty = 0.05f;

  genAlgoDef.randomGenomeChance = 0.1f;

  gero::rng::RNG::ensureRandomSeed();

  genAlgoDef.getRandomCallback = []()
  {
    return gero::rng::RNG::getNormalisedValue();
  };

  //
  //
  //

	ContinuousSpeciatedGeneticAlgorithm genAlgo;
  genAlgo.initialize(genAlgoDef);

	constexpr float k_maximumFitness = float(inAllTrainingData.size());
	constexpr float k_minimumFitness = k_maximumFitness - 0.05f;

  //
  //
  //

  // auto getTmpLog = [inTestName, &genAlgo, &genAlgoDef]()
  // {
  //   // return D_SSTR(
  //   //   // "##########"
  //   //   // << std::endl
  //   //   // << std::endl
  //   //   // << "Test Name: " << inTestName
  //   //   // << std::endl
  //   //   // << std::fixed << std::setprecision(2) << (float(genAlgo.getBestGenome().getFitness()) / k_minimumFitness) << "%"
  //   //   // << std::endl
  //   //   << "getTotalCompletedGenomes " << genAlgo.getTotalCompletedGenomes()
  //   //   // << std::endl
  //   //   << ", getTotalAncestors " << genAlgo.getTotalAncestors()
  //   //   // << std::endl
  //   //   // << "ancestorsWeightCoef " << genAlgoDef.ancestorsWeightCoef
  //   //   // << std::endl
  //   //   // << "TotalWeights " << genAlgoDef.topology.getTotalWeights()
  //   //   // << std::endl
  //   //   // << std::endl
  //   //   );
  //   return D_SSTR(
  //     "total completed genomes " << genAlgo.getTotalCompletedGenomes()
  //     << ", total ancestors " << genAlgo.getTotalAncestors()
  //     );
  // };

  std::string latestResult = "";

  auto printUpdate = [
    &inTestName,
    &genAlgo,
    k_minimumFitness,
    maxCompletedGenomes,
    &latestResult
  ]
  (bool isFinal)
  {
    const float currResult = (float(genAlgo.getBestGenome().getFitness()) / k_minimumFitness) * 100.0f;
    std::stringstream sstr;
    sstr << std::fixed << std::setprecision(2) << currResult << "%";
    std::string str = sstr.str();


    if (latestResult != str)
    {
      std::cerr << std::endl;
    }

    latestResult = str;

    std::cerr
      << "\r"
      << " => " << inTestName << " "
      << str
      << ", total completed genomes " << genAlgo.getTotalCompletedGenomes() << "/" << maxCompletedGenomes
      << " (" << std::fixed << std::setprecision(2) << (float(genAlgo.getTotalCompletedGenomes()) / float(maxCompletedGenomes)) * 100.0f << "%)"
      << ", total ancestors " << genAlgo.getTotalAncestors()
      // << std::endl
      ;

    if (isFinal)
    {
      std::cerr << std::endl;
    }
  };

  //
  //
  //

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
    {
      // std::cerr
      //   << "\r"
      //   << std::fixed << std::setprecision(2) << (float(genAlgo.getBestGenome().getFitness()) / k_minimumFitness) * 100.0f << "%"
      //   << ", total completed genomes " << genAlgo.getTotalCompletedGenomes() << "/" << maxCompletedGenomes
      //   << " (" << std::fixed << std::setprecision(2) << (float(genAlgo.getTotalCompletedGenomes()) / float(maxCompletedGenomes)) * 100.0f << "%)"
      //   << ", total ancestors " << genAlgo.getTotalAncestors()
      //   << std::endl
      //   ;

      printUpdate(true);

      break;
    }

    if ((genAlgo.getTotalCompletedGenomes() % 1000) == 0)
    {

      // D_MYERR(getTmpLog());

      // std::cerr
      //   << "\r"
      //   << std::fixed << std::setprecision(2) << (float(genAlgo.getBestGenome().getFitness()) / k_minimumFitness) * 100.0f << "%"
      //   << ", total completed genomes " << genAlgo.getTotalCompletedGenomes() << "/" << maxCompletedGenomes
      //   << " (" << std::fixed << std::setprecision(2) << (float(genAlgo.getTotalCompletedGenomes()) / float(maxCompletedGenomes)) * 100.0f << "%)"
      //   << ", total ancestors " << genAlgo.getTotalAncestors()
      //   // << std::endl
      //   ;

      printUpdate(false);

    }


    ASSERT_LE(genAlgo.getTotalCompletedGenomes(), maxCompletedGenomes)
      << std::endl
      << std::endl
      << " ===========> too slow, best was "
      << std::endl
      // << getTmpLog()
      // << std::endl
      ;

  }
  while (true);


  // {

  //   D_MYERR(getTmpLog());

  // }

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

TEST(continuous_genetic_algorithm, test_logic_gate___yes) {

  // D_MYERR("TEST <3");

  std::array<TrainingData<1, 1>, 2> allTrainingDatas = {{
    { {{ 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1, 1 });

  runTest_continuous("YES", allTrainingDatas, neuralNetworkTopology, 5 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___not) {

  std::array<TrainingData<1, 1>, 2> allTrainingDatas = {{
    { {{ 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1 });

  runTest_continuous("NOT", allTrainingDatas, neuralNetworkTopology, 5 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___and) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  runTest_continuous("AND", allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___nand) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 2, 1 });

  runTest_continuous("NAND", allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___or) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  runTest_continuous("OR", allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___nor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1 });

  runTest_continuous("NOR", allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___xor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 0.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 3, 1 });

  runTest_continuous("XOR", allTrainingDatas, neuralNetworkTopology, 20 * k_classicGenerationSize);
}

TEST(continuous_genetic_algorithm, test_logic_gate___xnor) {

  std::array<TrainingData<2, 1>, 4> allTrainingDatas = {{
    { {{ 0.0f, 0.0f }}, {{ 1.0f }} },
    { {{ 1.0f, 0.0f }}, {{ 0.0f }} },
    { {{ 0.0f, 1.0f }}, {{ 0.0f }} },
    { {{ 1.0f, 1.0f }}, {{ 1.0f }} },
  }};

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 3, 1 });

  runTest_continuous("XNOR", allTrainingDatas, neuralNetworkTopology, 50 * k_classicGenerationSize);
}


