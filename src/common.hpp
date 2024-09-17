#pragma once

#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "boost/graph/adjacency_list.hpp"
/**
 * @brief The minimum number of vertices
 */
#define MIN_VERTICES 2

/**
 * @brief The maximum number of vertices
 */
#define MAX_VERTICES 10000

/**
 * @brief The minimum number of edges
 */
#define MIN_EDGES 0

/**
 * @brief The maximum number of edges
 */
#define MAX_EDGES 100000

/**
 * Vertex properties
 */
struct vertex_properties_s
{
  /**
   * @brief The original number of the vertex in the input (1-indexed)
   */
  std::size_t number;

  /**
   * @brief Check if two vertices are equal
   * @param other The other vertex
   * @return True if the vertices are equal, false otherwise
   */
  bool operator==(const vertex_properties_s &other) const
  {
    return number == other.number;
  }

  /**
   * @brief Check if this vertex is less than another
   * @param other The other vertex
   * @return True if this vertex is less than the other, false otherwise
   */
  bool operator<(const vertex_properties_s &other) const
  {
    return number < other.number;
  }
};

namespace std
{
  template <>
  struct hash<vertex_properties_s>
  {
    /**
     * @brief Hash a vertex
     * @param v The vertex
     * @return The hash of the vertex
     */
    std::size_t operator()(const vertex_properties_s &v) const
    {
      return std::hash<std::size_t>()(v.number);
    }
  };
}

/**
 * @brief Unordered set of vertex properties
 */
typedef std::unordered_set<vertex_properties_s> unordered_vertex_properties_t;

/**
 * @brief Ordered vector of vertex properties
 */
typedef std::vector<vertex_properties_s> ordered_vertex_properties_t;

/**
 * @brief Adjacency list representation of the graph
 */
typedef boost::adjacency_list<boost::setS, boost::setS, boost::bidirectionalS, vertex_properties_s> graph_t;

/**
 * @brief Vertex descriptor
 */
typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor_t;

/**
 * @brief Vertex iterator
 */
typedef boost::graph_traits<graph_t>::vertex_iterator vertex_iterator_t;

/**
 * @brief Edge descriptor
 */
typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor_t;

/**
 * @brief Edge iterator
 */
typedef boost::graph_traits<graph_t>::edge_iterator edge_iterator_t;

/**
 * @brief Ordered vector of vertex descriptors
 */
typedef std::vector<vertex_descriptor_t> ordered_vertex_descriptors_t;

/**
 * @brief Map of vertices to their unnormalized traffic
 */
typedef std::unordered_map<vertex_descriptor_t, std::size_t> unnormalized_vertex_traffic_map_t;

/**
 * @brief Map of vertices to their normalized traffic
 */
typedef std::unordered_map<vertex_descriptor_t, double> normalized_vertex_traffic_map_t;

/**
 * @brief Ordered vector of graphs
 */
typedef std::vector<graph_t> ordered_graphs_t;
