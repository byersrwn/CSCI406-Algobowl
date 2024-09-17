#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "boost/graph/strong_components.hpp"
#include "boost/graph/topological_sort.hpp"
#include "boost/range/algorithm/sort.hpp"
#include "helpers.hpp"

/**
 * @brief The random number generator
 */
static boost::random::mt19937 rng(0);

std::size_t random_integer(const std::size_t start, const std::size_t end)
{
  // Initialize the distribution
  boost::random::uniform_int_distribution<std::size_t> distribution(start, end - 1);

  // Generate the random number
  std::size_t number = distribution(rng);

  return number;
}

void set_seed(const uint32_t seed)
{
  // Set the seed
  rng.seed(seed);
}

bool only_whitespace_remaining(std::istream &input)
{
  // Check if the only remaining characters are whitespace
  return std::all_of(
      std::istreambuf_iterator<char>(input),
      std::istreambuf_iterator<char>(),
      [](char c) {
        return std::isspace(c);
      });
}

ordered_graphs_t tarjans_subgraphs(const graph_t &graph)
{
  // Build the vertex to index bimap
  std::unordered_map<vertex_descriptor_t, std::size_t> vertexToIndex;
  std::unordered_map<std::size_t, vertex_descriptor_t> indexToVertex;

  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(graph)))
  {
    vertexToIndex.emplace(vertexDescriptor, vertexToIndex.size());
    indexToVertex.emplace(indexToVertex.size(), vertexDescriptor);
  }

  // Build the vertex index property map
  auto vertexToIndexPropertyMap = boost::make_assoc_property_map(vertexToIndex);
  std::vector<std::size_t> vertexIndexToComponentIndex(boost::num_vertices(graph));
  auto indexMap = boost::make_iterator_property_map(vertexIndexToComponentIndex.begin(), vertexToIndexPropertyMap);

  // Run Tarjan's algorithm
  std::size_t numSCCs = boost::strong_components(
      graph,
      indexMap,
      boost::vertex_index_map(vertexToIndexPropertyMap));

  // Build the subgraphs
  ordered_graphs_t subgraphs;
  subgraphs.reserve(numSCCs);
  for (std::size_t i = 0; i < numSCCs; i++)
  {
    subgraphs.push_back(graph_t());
  }

  /**
   * @brief The vertex index to subgraph vertex map
   */
  std::unordered_map<std::size_t, vertex_descriptor_t> vertexIndexToSubgraphVertex(numSCCs);

  /**
   * @brief The vertex indices for each component (relative to the input graph)
   */
  std::vector<std::unordered_set<std::size_t>> componentsVerticesIndex(numSCCs);

  // Group vertices by component and add them to the subgraphs
  for (std::size_t i = 0; i < vertexToIndex.size(); i++)
  {
    // Get information
    const auto componentIndex = vertexIndexToComponentIndex[i];
    const auto vertexDescriptor = indexToVertex[i];
    const auto vertexProperties = graph[vertexDescriptor];

    // Add the vertex index to the component vertices index
    componentsVerticesIndex[componentIndex].insert(i);

    // Add the vertex to the subgraph
    const auto vertex = boost::add_vertex(vertexProperties, subgraphs[componentIndex]);

    // Add the vertex to the subgraph vertex map
    vertexIndexToSubgraphVertex[i] = vertex;
  }

  // Add edges to the subgraphs
  for (const auto &edgeDescriptor : boost::make_iterator_range(boost::edges(graph)))
  {
    // Get the original vertex descriptors
    const auto originalSourceVertexDescriptor = boost::source(edgeDescriptor, graph);
    const auto originalTargetVertexDescriptor = boost::target(edgeDescriptor, graph);

    // Get the vertex indices
    const auto sourceVertexIndex = vertexToIndex[originalSourceVertexDescriptor];
    const auto targetVertexIndex = vertexToIndex[originalTargetVertexDescriptor];

    // Get the component indices
    const auto sourceComponentIndex = vertexIndexToComponentIndex[sourceVertexIndex];
    const auto targetComponentIndex = vertexIndexToComponentIndex[targetVertexIndex];

    // Add the edge to the subgraph only if the source and target are in the same component
    if (sourceComponentIndex == targetComponentIndex)
    {
      // Add the edge
      boost::add_edge(
          vertexIndexToSubgraphVertex[sourceVertexIndex],
          vertexIndexToSubgraphVertex[targetVertexIndex],
          subgraphs[sourceComponentIndex]);
    }
  }

  return subgraphs;
}

bool detect_cycles(const graph_t &graph)
{
  // Build the vertex to index map
  std::unordered_map<vertex_descriptor_t, std::size_t> vertexToIndex;

  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(graph)))
  {
    vertexToIndex.emplace(vertexDescriptor, vertexToIndex.size());
  }

  // Build the vertex index property map
  auto vertexToIndexPropertyMap = boost::make_assoc_property_map(vertexToIndex);
  std::vector<std::size_t> finishTimes(boost::num_vertices(graph));

  // Run topological sort
  std::vector<vertex_descriptor_t> sortedVertices;
  try
  {
    boost::topological_sort(
        graph,
        std::back_inserter(sortedVertices),
        boost::vertex_index_map(vertexToIndexPropertyMap));
  }
  catch (const boost::not_a_dag &e)
  {
    return true;
  }

  return false;
}
