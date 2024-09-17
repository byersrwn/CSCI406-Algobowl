#include <iostream>
#include <stdexcept>
#include <vector>

#include "boost/random.hpp"
#include "helpers.hpp"
#include "simulation.hpp"

unnormalized_vertex_traffic_map_t simulate(const graph_t &component, const std::size_t agents, const std::size_t steps, const std::size_t batches, const double change_threshold)
{
  // Initialize traffic
  unnormalized_vertex_traffic_map_t unnormalizedTraffic;
  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(component)))
  {
    unnormalizedTraffic[vertexDescriptor] = 0;
  }

  // Build a map from vertices to out-vertices
  std::unordered_map<vertex_descriptor_t, std::vector<vertex_descriptor_t>> vertexToOutVertices;
  std::unordered_map<std::size_t, vertex_descriptor_t> indexToVertex;

  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(component)))
  {
    indexToVertex.emplace(indexToVertex.size(), vertexDescriptor);
    for (const auto &edgeDescriptor : boost::make_iterator_range(boost::out_edges(vertexDescriptor, component)))
    {
      vertexToOutVertices[vertexDescriptor].push_back(boost::target(edgeDescriptor, component));
    }
  }

  // Iterate over batches
  normalized_vertex_traffic_map_t previousNormalizedTraffic{};
  for (std::size_t batch = 0; batch < batches; batch++)
  {
    // Iterate over agents
    for (std::size_t agent = 0; agent < agents; agent++)
    {
      // Initialize the agent with a random start vertex
      std::size_t currentVertexIndex = random_integer(0, boost::num_vertices(component));
      vertex_descriptor_t currentVertex = indexToVertex[currentVertexIndex];

      // Iterate over steps
      for (std::size_t step = 0; step < steps; step++)
      {
        // Get the out-edges of the current vertex
        const auto outEdges = vertexToOutVertices[currentVertex];
        const auto outDegree = outEdges.size();

        // Get the next vertex
        vertex_descriptor_t nextVertex;

        if (outDegree == 0)
        {
          throw std::invalid_argument("The graph is not strongly connected");
        }
        else if (outDegree == 1)
        {
          nextVertex = outEdges[0];
        }
        else
        {
          nextVertex = outEdges[random_integer(0, outDegree)];
        }

        // Update the traffic
        unnormalizedTraffic[nextVertex]++;

        // Move to the next vertex
        currentVertex = nextVertex;
      }
    }

    // Compute the mean normalized traffic difference and update the previous normalized traffic
    std::size_t totalTraffic = (batch + 1) * agents * steps;
    double meanNormalizedTrafficDifference = 0;

    for (const auto &[vertexDescriptor, traffic] : unnormalizedTraffic)
    {
      // Get the normalized traffics
      double oldNormalizedTraffic = previousNormalizedTraffic.contains(vertexDescriptor) ? previousNormalizedTraffic[vertexDescriptor] : 0;
      double newNormalizedTraffic = (double)traffic / (double)totalTraffic;

      // Update the mean normalized traffic difference
      meanNormalizedTrafficDifference += std::abs(newNormalizedTraffic - oldNormalizedTraffic);

      // Update the previous normalized traffic
      previousNormalizedTraffic[vertexDescriptor] = newNormalizedTraffic;
    }

    // Print the mean normalized traffic difference
    std::cout << "Processed batch " << batch + 1 << " of at most " << batches << " with mean normalized traffic difference " << meanNormalizedTrafficDifference << " (>=" << (batch + 1) * 100 / batches << "%, threshold: " << change_threshold << ", agents/batch: " << agents << ", steps/agent/batch: " << steps << ")" << std::endl;

    // Terminate early if the mean normalized traffic difference is below the proportionality change threshold
    if (meanNormalizedTrafficDifference < change_threshold)
    {
      std::cout << "Terminating early" << std::endl;
      break;
    }
  }

  return unnormalizedTraffic;
}
