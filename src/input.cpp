#include <iterator>
#include <map>
#include <stdexcept>
#include <vector>

#include "input.hpp"
#include "helpers.hpp"

graph_t deserialize_input(std::istream &input)
{
  // Read the number of vertices
  std::size_t numVertices;
  if (!(input >> numVertices))
  {
    throw std::invalid_argument("The input file does not contain the number of vertices");
  }

  // Ensure the number of vertices is valid
  if (numVertices < MIN_VERTICES || MAX_VERTICES < numVertices)
  {
    throw std::invalid_argument("The number of vertices must be between " + std::to_string(MIN_VERTICES) + " and " + std::to_string(MAX_VERTICES));
  }

  graph_t graph;

  // Add the vertices
  std::vector<vertex_descriptor_t> vertices;
  vertices.reserve(numVertices);
  for (std::size_t vertexIndex = 1; vertexIndex <= numVertices; vertexIndex++)
  {
    vertices.push_back(boost::add_vertex(vertex_properties_s{vertexIndex}, graph));
  }

  // Read the edges
  for (std::size_t destinationIndex = 0; destinationIndex < numVertices; destinationIndex++)
  {
    // Read the number of in vertices
    std::size_t inVertexCount;
    if (!(input >> inVertexCount))
    {
      throw std::invalid_argument("The input file does not contain the number of in vertices for vertex " + std::to_string(destinationIndex) + " (0-indexed)");
    }

    if (numVertices < inVertexCount)
    {
      throw std::invalid_argument("The number of in vertices must be between 0 and " + std::to_string(numVertices) + " for vertex " + std::to_string(destinationIndex) + " (0-indexed)");
    }

    // Read the adjacent vertices
    for (std::size_t adjacentIndex = 0; adjacentIndex < inVertexCount; adjacentIndex++)
    {
      // Read the source index
      std::size_t sourceIndex;
      if (!(input >> sourceIndex))
      {
        throw std::invalid_argument("The input file does not contain the source index for vertex " + std::to_string(destinationIndex) + " (0-indexed)");
      }

      if (sourceIndex < 1 && numVertices < sourceIndex)
      {
        throw std::invalid_argument("The destination index must be between 1 and " + std::to_string(numVertices) + " for vertex " + std::to_string(destinationIndex) + " (0-indexed)");
      }

      sourceIndex--;

      // Lookup the vertices
      const auto source = vertices[sourceIndex];
      const auto target = vertices[destinationIndex];

      // Add the edge to the graph
      boost::add_edge(source, target, graph);
    }
  }

  // Check if the stream only has whitespace remaining
  if (!only_whitespace_remaining(input))
  {
    throw std::invalid_argument("The input file contains extra data");
  }

  // Ensure the number of edges is valid
  std::size_t numEdges = boost::num_edges(graph);
  if (numEdges < MIN_EDGES || MAX_EDGES < numEdges)
  {
    throw std::invalid_argument("The number of edges must be between " + std::to_string(MIN_EDGES) + " and " + std::to_string(MAX_EDGES));
  }

  return graph;
}

void serialize_input(std::ostream &output, const graph_t &graph)
{
  // Extract the original number and sort the graph
  std::map<std::size_t, std::set<std::size_t>> sortedGraph;
  for (const auto &vertexDescriptor : boost::make_iterator_range(boost::vertices(graph)))
  {
    // Iterate over in-edges
    for (const auto &edgeDescriptor : boost::make_iterator_range(boost::in_edges(vertexDescriptor, graph)))
    {
      const auto source = graph[boost::source(edgeDescriptor, graph)];
      const auto target = graph[boost::target(edgeDescriptor, graph)];

      sortedGraph[target.number].insert(source.number);
    }
  }

  // Write the number of vertices
  if (!(output << boost::num_vertices(graph) << std::endl))
  {
    throw std::runtime_error("Failed to write the number of vertices");
  }

  // Write the vertices
  for (auto entry = sortedGraph.begin(); entry != sortedGraph.end(); entry++)
  {
    // Write the number of sources
    if (!(output << entry->second.size() << " "))
    {
      throw std::runtime_error("Failed to write the number of sources for vertex " + std::to_string(entry->first));
    }

    // Write the sources
    for (auto source = entry->second.begin(); source != entry->second.end(); source++)
    {
      if (!(output << *source))
      {
        throw std::runtime_error("Failed to write the source for vertex " + std::to_string(entry->first));
      }

      if (std::next(source) != entry->second.end())
      {
        if (!(output << " "))
        {
          throw std::runtime_error("Failed to write the source for vertex " + std::to_string(entry->first));
        }
      }
    }

    if (std::next(entry) != sortedGraph.end())
    {
      if (!(output << std::endl))
      {
        throw std::runtime_error("Failed to write the source for vertex " + std::to_string(entry->first));
      }
    }
  }
}
