#include <gtest/gtest.h>
#include <fstream>
#include <sstream>

#include "output.hpp"

TEST(deserialize_output, sample_0)
{
  // Open the file
  std::ifstream file("test/0-sample-out-provided-0.txt");

  // Deserialize the vertices
  const auto vertices = deserialize_output(file);

  // Assert the vertices
  ASSERT_EQ(vertices.size(), 2);
  ASSERT_EQ(vertices.count(vertex_properties_s{3}), 1);
  ASSERT_EQ(vertices.count(vertex_properties_s{5}), 1);

  // Cleanup
  file.close();
}

TEST(deserialize_output, sample_1)
{
  // Open the file
  std::ifstream file("test/0-sample-out-provided-1.txt");

  // Deserialize the vertices
  const auto vertices = deserialize_output(file);

  // Assert the vertices
  ASSERT_EQ(vertices.size(), 1);
  ASSERT_EQ(vertices.count(vertex_properties_s{1}), 1);

  // Cleanup
  file.close();
}

TEST(serialize_output, sample_0)
{
  // Construct the vertices
  unordered_vertex_properties_t vertices = {vertex_properties_s{3}, vertex_properties_s{5}};

  // Serialize the vertices
  std::ostringstream serialized;
  serialize_output(serialized, vertices);

  // Assert the serialization
  ASSERT_EQ(serialized.str() == "2\n3 5" || serialized.str() == "2\n5 3", true);
}

TEST(serialize_output, sample_1)
{ // Construct the vertices
  unordered_vertex_properties_t vertices = {vertex_properties_s{1}};

  // Serialize the vertices
  std::ostringstream serialized;
  serialize_output(serialized, vertices);

  // Assert the serialization
  ASSERT_EQ(serialized.str(), "1\n1");
}
