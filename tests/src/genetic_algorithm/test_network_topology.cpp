
#include "headers.hpp"

TEST(neural_network_topology, test_topology__input_1__output_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 1, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInputLayerSize(), 1);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().size(), 0);
  ASSERT_EQ(neuralNetworkTopology.getOutputLayerSize(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalLayers(), 2);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 2);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 1);
}

TEST(neural_network_topology, test_topology__input_2__output_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInputLayerSize(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().size(), 0);
  ASSERT_EQ(neuralNetworkTopology.getOutputLayerSize(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalLayers(), 2);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 3);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 2);
}

TEST(neural_network_topology, test_topology__input_2__hidden_1__output_1) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 2, 1, 1 });

  ASSERT_EQ(neuralNetworkTopology.getInputLayerSize(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().size(), 1);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().at(0), 1);
  ASSERT_EQ(neuralNetworkTopology.getOutputLayerSize(), 1);
  ASSERT_EQ(neuralNetworkTopology.getTotalLayers(), 3);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 4);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), 2 + 1);
}


TEST(neural_network_topology, test_topology__input_5__hidden_10__hidden_10__output_3) {

  NeuralNetworkTopology neuralNetworkTopology;
  neuralNetworkTopology.init({ 5, 10, 10, 3 });

  ASSERT_EQ(neuralNetworkTopology.getInputLayerSize(), 5);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().size(), 2);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().at(0), 10);
  ASSERT_EQ(neuralNetworkTopology.getHiddenLayers().at(1), 10);
  ASSERT_EQ(neuralNetworkTopology.getOutputLayerSize(), 3);
  ASSERT_EQ(neuralNetworkTopology.getTotalLayers(), 4);
  ASSERT_EQ(neuralNetworkTopology.getTotalNeurons(), 28);
  ASSERT_EQ(neuralNetworkTopology.getTotalWeights(), (5 * 10) + (10 * 10) + (10 * 3));
}

