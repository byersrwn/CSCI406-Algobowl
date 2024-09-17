#include <algorithm>
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "input.hpp"

/**
 * @brief Get the out vertices adjacent to the vertex
 * @param graph The graph
 * @param vertex The vertex properties
 * @param outEdgesVector The out edges vector
 */
static void getOutVertices(const graph_t &graph, vertex_properties_s vertexProperties, ordered_vertex_properties_t &outEdgesVector)
{
  ASSERT_TRUE(outEdgesVector.size() == 0);

  // Get the vertex
  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(graph)))
  {
    if (graph[vertexDescriptor] == vertexProperties)
    {
      // Copy to a vector
      outEdgesVector.reserve(boost::out_degree(vertexDescriptor, graph));

      for (const auto &edgeDescriptor : boost::make_iterator_range(boost::out_edges(vertexDescriptor, graph)))
      {
        outEdgesVector.push_back(graph[boost::target(edgeDescriptor, graph)]);
      }

      // Sort
      std::sort(outEdgesVector.begin(), outEdgesVector.end());

      return;
    }
  }

  FAIL() << "The vertex with number " << vertexProperties.number << " was not found in the graph";
}

TEST(deserialize_input, sample)
{
  // Open the file
  std::ifstream file("test/0-sample-in.txt");

  // Deserialize the graph
  const auto graph = deserialize_input(file);

  // Assert the graph
  ASSERT_EQ(boost::num_vertices(graph), 5);
  ASSERT_EQ(boost::num_edges(graph), 6);

  ordered_vertex_properties_t vertex1Out;
  getOutVertices(graph, vertex_properties_s{1}, vertex1Out);
  ASSERT_EQ(vertex1Out.size(), 2);
  ASSERT_EQ(vertex1Out[0], vertex_properties_s{2});
  ASSERT_EQ(vertex1Out[1], vertex_properties_s{4});

  ordered_vertex_properties_t vertex2Out;
  getOutVertices(graph, vertex_properties_s{2}, vertex2Out);
  ASSERT_EQ(vertex2Out.size(), 1);
  ASSERT_EQ(vertex2Out[0], vertex_properties_s{3});

  ordered_vertex_properties_t vertex3Out;
  getOutVertices(graph, vertex_properties_s{3}, vertex3Out);
  ASSERT_EQ(vertex3Out.size(), 1);
  ASSERT_EQ(vertex3Out[0], vertex_properties_s{1});

  ordered_vertex_properties_t vertex4Out;
  getOutVertices(graph, vertex_properties_s{4}, vertex4Out);
  ASSERT_EQ(vertex4Out.size(), 1);
  ASSERT_EQ(vertex4Out[0], vertex_properties_s{5});

  ordered_vertex_properties_t vertex5Out;
  getOutVertices(graph, vertex_properties_s{5}, vertex5Out);
  ASSERT_EQ(vertex5Out.size(), 1);
  ASSERT_EQ(vertex5Out[0], vertex_properties_s{1});

  // Cleanup
  file.close();
}

TEST(serialize_input, sample)
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

  // Serialize the graph
  std::ostringstream serialized;
  serialize_input(serialized, graph);
  auto str = serialized.str();

  // Assert the serialization
  ASSERT_EQ(str == "5\n2 3 5\n1 1\n1 2\n1 1\n1 4" || str == "5\n2 5 3\n1 1\n1 2\n1 1\n1 4", true) << "The serialization is " << str;
}
