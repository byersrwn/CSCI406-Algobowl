#include <gtest/gtest.h>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include "helpers.hpp"

/**
 * @brief Mapsort test map
 */
typedef std::unordered_map<int, int> mapsort_test_map_t;

/**
 * @brief Mapsort test vector
 */
typedef std::vector<int> mapsort_test_vector_t;

/**
 * @brief Expected subgraph (Vertex mapped to all out vertices)
 */
typedef std::map<vertex_properties_s, std::set<vertex_properties_s>> expected_subgraph_t;

/**
 * @brief Expected subgraphs (Set of vertices mapped to all out vertices)
 */
typedef std::set<expected_subgraph_t> expected_subgraphs_t;

/**
 * @brief Compare two integers such that they are sorted in ascscending order
 * @param a The first integer
 * @param b The second integer
 */
static bool intCompare(const int &a, const int &b)
{
  return a < b;
}

/**
 * @brief Assert the specified subgraphs match the expected subgraphs
 * @param graphs The subgraphs
 * @param expected The expected subgraphs (Set of vertices mapped to all out vertices)
 */
static void assertSubGraphs(const ordered_graphs_t &graphs, const expected_subgraphs_t &expected)
{
  ASSERT_EQ(graphs.size(), expected.size());

  expected_subgraphs_t actual;
  for (std::size_t i = 0; i < graphs.size(); i++)
  {
    // Get the subgraph
    const auto subgraph = graphs[i];

    // Build the expected subgraph
    expected_subgraph_t expectedSubgraph;
    for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(subgraph)))
    {
      // Get the source vertex
      const auto source = subgraph[vertexDescriptor];

      // Get the target vertices
      std::set<vertex_properties_s> targetVertices;
      for (const auto &edgeDescriptor : boost::make_iterator_range(boost::out_edges(vertexDescriptor, subgraph)))
      {
        targetVertices.insert(subgraph[boost::target(edgeDescriptor, subgraph)]);
      }

      // Add the vertex to the map
      expectedSubgraph[source] = targetVertices;
    }

    // Add the map to the set
    actual.insert(expectedSubgraph);
  }

  ASSERT_EQ(actual, expected);
}

TEST(only_whitespace_remaining, empty)
{
  // Construct the input stream
  std::istringstream input("");

  // Check if only whitespace is remaining
  const auto result = only_whitespace_remaining(input);

  // Assert the result
  ASSERT_TRUE(result);
}

TEST(only_whitespace_remaining, text)
{
  // Construct the input stream
  std::istringstream input("abc");

  // Check if only whitespace is remaining
  const auto result = only_whitespace_remaining(input);

  // Assert the result
  ASSERT_FALSE(result);
}

TEST(only_whitespace_remaining, whitespace)
{
  // Construct the input stream
  std::istringstream input(" \t\n");

  // Check if only whitespace is remaining
  const auto result = only_whitespace_remaining(input);

  // Assert the result
  ASSERT_TRUE(result);
}

TEST(only_whitespace_remaining, whitespace_and_text)
{
  // Construct the input stream
  std::istringstream input(" \t\nabc");

  // Check if only whitespace is remaining
  const auto result = only_whitespace_remaining(input);

  // Assert the result
  ASSERT_FALSE(result);
}

TEST(tarjans_subgraphs, one_scc)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);
  const auto vertex5 = boost::add_vertex(vertex_properties_s{5}, graph);

  boost::add_edge(vertex3, vertex1, graph);
  boost::add_edge(vertex5, vertex1, graph);
  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex1, vertex4, graph);
  boost::add_edge(vertex4, vertex5, graph);

  // Run Tarjan's algorithm
  const auto graphs = tarjans_subgraphs(graph);

  // Assert the graphs
  expected_subgraphs_t expected = {
      {
          {vertex_properties_s{1}, {vertex_properties_s{2}, vertex_properties_s{4}}},
          {vertex_properties_s{2}, {vertex_properties_s{3}}},
          {vertex_properties_s{3}, {vertex_properties_s{1}}},
          {vertex_properties_s{4}, {vertex_properties_s{5}}},
          {vertex_properties_s{5}, {vertex_properties_s{1}}},
      },
  };

  assertSubGraphs(graphs, expected);
}

TEST(tarjans_subgraphs, all_sccs)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);
  const auto vertex5 = boost::add_vertex(vertex_properties_s{5}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex4, vertex5, graph);

  // Run Tarjan's algorithm
  const auto graphs = tarjans_subgraphs(graph);

  // Assert the graphs
  expected_subgraphs_t expected = {
      {
          {vertex_properties_s{1}, {}},
      },
      {
          {vertex_properties_s{2}, {}},
      },
      {
          {vertex_properties_s{3}, {}},
      },
      {
          {vertex_properties_s{4}, {}},
      },
      {
          {vertex_properties_s{5}, {}},
      },
  };

  assertSubGraphs(graphs, expected);
}

TEST(mapsort, empty_map)
{
  // Construct the map
  mapsort_test_map_t map;

  // Sort the map
  const auto keys = mapsort<int, int>(map, intCompare);

  // Assert the keys
  ASSERT_EQ(keys.size(), 0);
}

TEST(mapsort, one_item)
{
  // Construct the map
  mapsort_test_map_t map;
  map[1] = 2;

  // Sort the map
  const auto keys = mapsort<int, int>(map, intCompare);

  // Assert the keys
  ASSERT_EQ(keys.size(), 1);
  ASSERT_EQ(keys[0], 1);
}

TEST(mapsort, multiple_ascending_items)
{
  // Construct the map
  mapsort_test_map_t map;
  map[1] = 1;
  map[2] = 2;
  map[3] = 3;

  // Sort the map
  const auto keys = mapsort<int, int>(map, intCompare);

  // Assert the keys
  ASSERT_EQ(keys.size(), 3);
  ASSERT_EQ(keys[0], 1);
  ASSERT_EQ(keys[1], 2);
  ASSERT_EQ(keys[2], 3);
}

TEST(mapsort, multiple_descending_items)
{
  // Construct the map
  mapsort_test_map_t map;
  map[1] = 3;
  map[2] = 2;
  map[3] = 1;

  // Sort the map
  const auto keys = mapsort<int, int>(map, intCompare);

  // Assert the keys
  ASSERT_EQ(keys.size(), 3);
  ASSERT_EQ(keys[0], 3);
  ASSERT_EQ(keys[1], 2);
  ASSERT_EQ(keys[2], 1);
}

TEST(mapsort, multiple_random_items)
{
  // Construct the map
  mapsort_test_map_t map;
  map[1] = 5;
  map[2] = 1;
  map[3] = 2;
  map[4] = 4;
  map[5] = 3;

  // Sort the map
  const auto keys = mapsort<int, int>(map, intCompare);

  // Assert the keys
  ASSERT_EQ(keys.size(), 5);
  ASSERT_EQ(keys[0], 2);
  ASSERT_EQ(keys[1], 3);
  ASSERT_EQ(keys[2], 5);
  ASSERT_EQ(keys[3], 4);
  ASSERT_EQ(keys[4], 1);
}

TEST(detect_cycles, one_vertex)
{
  // Construct the graph
  graph_t graph;

  boost::add_vertex(vertex_properties_s{1}, graph);

  // Detect cycles
  const auto hasCycles = detect_cycles(graph);

  // Assert the result
  ASSERT_FALSE(hasCycles);
}

TEST(detect_cycles, one_edge)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);

  boost::add_edge(vertex1, vertex2, graph);

  // Detect cycles
  const auto hasCycles = detect_cycles(graph);

  // Assert the result
  ASSERT_FALSE(hasCycles);
}

TEST(detect_cycles, two_edges)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex2, vertex3, graph);

  // Detect cycles
  const auto hasCycles = detect_cycles(graph);

  // Assert the result
  ASSERT_FALSE(hasCycles);
}

TEST(detect_cycles, one_cycle)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex3, vertex1, graph);

  // Detect cycles
  const auto hasCycles = detect_cycles(graph);

  // Assert the result
  ASSERT_TRUE(hasCycles);
}

TEST(detect_cycles, two_cycles)
{
  // Construct the graph
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);
  const auto vertex5 = boost::add_vertex(vertex_properties_s{5}, graph);

  boost::add_edge(vertex3, vertex1, graph);
  boost::add_edge(vertex5, vertex1, graph);
  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex1, vertex4, graph);
  boost::add_edge(vertex4, vertex5, graph);

  // Detect cycles
  const auto hasCycles = detect_cycles(graph);

  // Assert the result
  ASSERT_TRUE(hasCycles);
}
