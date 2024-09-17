#include <iterator>
#include <stdexcept>

#include "helpers.hpp"
#include "output.hpp"

unordered_vertex_properties_t deserialize_output(std::istream &input)
{
  unordered_vertex_properties_t vertices;

  // Read the number of vertices
  std::size_t numVertices;
  if (!(input >> numVertices))
  {
    throw std::invalid_argument("The input file does not contain the number of vertices");
  }

  if (MAX_VERTICES < numVertices)
  {
    throw std::invalid_argument("The number of vertices must be between 0 and " + std::to_string(MAX_VERTICES));
  }

  vertices.reserve(numVertices);

  // Read the vertices
  for (std::size_t vertexIndex = 0; vertexIndex < numVertices; vertexIndex++)
  {
    // Read the vertex index
    std::size_t vertex;
    if (!(input >> vertex))
    {
      throw std::invalid_argument("The input file does not contain the vertex index for vertex " + std::to_string(vertexIndex) + " (0-indexed)");
    }

    // Add the vertex to the vertices
    vertices.insert(vertex_properties_s{vertex});
  }

  // Check if the stream only has whitespace remaining
  if (!only_whitespace_remaining(input))
  {
    throw std::invalid_argument("The input file contains extra data");
  }

  return vertices;
}

void serialize_output(std::ostream &output, const unordered_vertex_properties_t &vertices)
{
  // Write the number of vertices
  if (!(output << vertices.size() << std::endl))
  {
    throw std::runtime_error("Failed to write the number of vertices");
  }

  // Write the vertices
  for (auto vertex = vertices.begin(); vertex != vertices.end(); vertex++)
  {
    if (!(output << vertex->number))
    {
      throw std::runtime_error("Failed to write the vertex");
    }

    if (std::next(vertex) != vertices.end())
    {
      if (!(output << " "))
      {
        throw std::runtime_error("Failed to write the space");
      }
    }
  }
}
