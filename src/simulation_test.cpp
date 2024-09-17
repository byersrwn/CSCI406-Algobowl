#include <gtest/gtest.h>
#include <tuple>
#include <vector>

#include "helpers.hpp"
#include "simulation.hpp"

/**
 * @brief Build a graph with a single strongly connected component
 * @return The graph and the vertices
 */
static std::tuple<graph_t, std::vector<vertex_descriptor_t>> build_one_scc()
{
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

  return {graph, {vertex1, vertex2, vertex3, vertex4, vertex5}};
}

/**
 * @brief Build a graph with two strongly connected components
 * @return The graph and the vertices
 */
static std::tuple<graph_t, std::vector<vertex_descriptor_t>> build_two_sccs()
{
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);
  const auto vertex5 = boost::add_vertex(vertex_properties_s{5}, graph);
  const auto vertex6 = boost::add_vertex(vertex_properties_s{6}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex1, vertex3, graph);
  boost::add_edge(vertex1, vertex4, graph);
  boost::add_edge(vertex1, vertex5, graph);
  boost::add_edge(vertex1, vertex6, graph);

  boost::add_edge(vertex6, vertex1, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex3, vertex4, graph);
  boost::add_edge(vertex4, vertex5, graph);
  boost::add_edge(vertex5, vertex6, graph);
  boost::add_edge(vertex6, vertex2, graph);

  return {graph, {vertex1, vertex2, vertex3, vertex4, vertex5, vertex6}};
}

/**
 * @brief Build a graph with a loop
 * @return The graph and the vertices
 */
static std::tuple<graph_t, std::vector<vertex_descriptor_t>> build_loop()
{
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex1, vertex3, graph);
  boost::add_edge(vertex1, vertex4, graph);

  boost::add_edge(vertex2, vertex1, graph);
  boost::add_edge(vertex3, vertex1, graph);
  boost::add_edge(vertex4, vertex1, graph);

  return {graph, {vertex1, vertex2, vertex3, vertex4}};
}

/**
 * @brief Build a fully connected graph
 * @return The graph and the vertices
 */
static std::tuple<graph_t, std::vector<vertex_descriptor_t>> build_fully_connected()
{
  graph_t graph;

  const auto vertex1 = boost::add_vertex(vertex_properties_s{1}, graph);
  const auto vertex2 = boost::add_vertex(vertex_properties_s{2}, graph);
  const auto vertex3 = boost::add_vertex(vertex_properties_s{3}, graph);
  const auto vertex4 = boost::add_vertex(vertex_properties_s{4}, graph);
  const auto vertex5 = boost::add_vertex(vertex_properties_s{5}, graph);
  const auto vertex6 = boost::add_vertex(vertex_properties_s{6}, graph);

  boost::add_edge(vertex1, vertex2, graph);
  boost::add_edge(vertex1, vertex3, graph);
  boost::add_edge(vertex1, vertex4, graph);
  boost::add_edge(vertex1, vertex5, graph);
  boost::add_edge(vertex1, vertex6, graph);

  boost::add_edge(vertex2, vertex1, graph);
  boost::add_edge(vertex2, vertex3, graph);
  boost::add_edge(vertex2, vertex4, graph);
  boost::add_edge(vertex2, vertex5, graph);
  boost::add_edge(vertex2, vertex6, graph);

  boost::add_edge(vertex3, vertex1, graph);
  boost::add_edge(vertex3, vertex2, graph);
  boost::add_edge(vertex3, vertex4, graph);
  boost::add_edge(vertex3, vertex5, graph);
  boost::add_edge(vertex3, vertex6, graph);

  boost::add_edge(vertex4, vertex1, graph);
  boost::add_edge(vertex4, vertex2, graph);
  boost::add_edge(vertex4, vertex3, graph);
  boost::add_edge(vertex4, vertex5, graph);
  boost::add_edge(vertex4, vertex6, graph);

  boost::add_edge(vertex5, vertex1, graph);
  boost::add_edge(vertex5, vertex2, graph);
  boost::add_edge(vertex5, vertex3, graph);
  boost::add_edge(vertex5, vertex4, graph);
  boost::add_edge(vertex5, vertex6, graph);

  boost::add_edge(vertex6, vertex1, graph);
  boost::add_edge(vertex6, vertex2, graph);
  boost::add_edge(vertex6, vertex3, graph);
  boost::add_edge(vertex6, vertex4, graph);
  boost::add_edge(vertex6, vertex5, graph);

  return {graph, {vertex1, vertex2, vertex3, vertex4, vertex5, vertex6}};
}

TEST(simulate, one_scc_0)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_one_scc();

  // Seed the RNG
  set_seed(3);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 2, 8, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 6},
      {vertices[1], 3},
      {vertices[2], 2},
      {vertices[3], 2},
      {vertices[4], 3},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, one_scc_1)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_one_scc();

  // Seed the RNG
  set_seed(8);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 4, 20, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 27},
      {vertices[1], 15},
      {vertices[2], 15},
      {vertices[3], 11},
      {vertices[4], 12},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, two_sccs_0)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_two_sccs();

  // Seed the RNG
  set_seed(16);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 2, 10, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 1},
      {vertices[1], 3},
      {vertices[2], 3},
      {vertices[3], 3},
      {vertices[4], 5},
      {vertices[5], 5},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, two_sccs_1)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_two_sccs();

  // Seed the RNG
  set_seed(16);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 4, 30, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 12},
      {vertices[1], 17},
      {vertices[2], 19},
      {vertices[3], 20},
      {vertices[4], 24},
      {vertices[5], 28},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, two_sccs_2)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_two_sccs();

  // Seed the RNG
  set_seed(1286);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 10, 120, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 121},
      {vertices[1], 166},
      {vertices[2], 191},
      {vertices[3], 220},
      {vertices[4], 243},
      {vertices[5], 259},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, loop_0)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_loop();

  // Seed the RNG
  set_seed(109237810);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 1, 5, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 3},
      {vertices[1], 1},
      {vertices[2], 1},
      {vertices[3], 0},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, loop_1)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_loop();

  // Seed the RNG
  set_seed(109237810);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 1000, 50, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 25000},
      {vertices[1], 8270},
      {vertices[2], 8330},
      {vertices[3], 8400},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, fully_connected_0)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_fully_connected();

  // Seed the RNG
  set_seed(182736);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 2, 10, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 5},
      {vertices[1], 4},
      {vertices[2], 1},
      {vertices[3], 6},
      {vertices[4], 1},
      {vertices[5], 3},
  };

  ASSERT_EQ(traffic, expected);
}

TEST(simulate, fully_connected_1)
{
  // Build the graph
  graph_t graph;
  std::vector<vertex_descriptor_t> vertices;
  std::tie(graph, vertices) = build_fully_connected();

  // Seed the RNG
  set_seed(109237810);

  // Run the simulation
  unnormalized_vertex_traffic_map_t traffic = simulate(graph, 20, 100, 1, 0.0);

  // Assert the traffic
  unnormalized_vertex_traffic_map_t expected = {
      {vertices[0], 348},
      {vertices[1], 329},
      {vertices[2], 345},
      {vertices[3], 316},
      {vertices[4], 329},
      {vertices[5], 333},
  };

  ASSERT_EQ(traffic, expected);
}
