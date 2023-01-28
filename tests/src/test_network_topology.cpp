
#include "headers.hpp"

TEST(test_genetic_algorithm, test_topology__1_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInput(), 1);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().size(), 0);
  ASSERT_EQ(neuralNetworkTopology.getOutput(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 2);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 1);
}

TEST(test_genetic_algorithm, test_topology__2_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInput(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().size(), 0);
  ASSERT_EQ(neuralNetworkTopology.getOutput(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 3);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 2);
}

TEST(test_genetic_algorithm, test_topology__2_1_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInput(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().size(), 1);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().at(0), 1);
  ASSERT_EQ(neuralNetworkTopology.getOutput(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 4);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 2 + 1);
}


TEST(test_genetic_algorithm, test_topology__5_10_10_3) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 5, 10, 10, 3 });

  ASSERT_EQ(neuralNetworkTopology.getInput(), 5);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().size(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().at(0), 10);
  ASSERT_EQ(neuralNetworkTopology.getHiddens().at(1), 10);
  ASSERT_EQ(neuralNetworkTopology.getOutput(), 3);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 28);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 50 + 100 + 30);
}

