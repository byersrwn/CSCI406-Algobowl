#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/random.hpp"
#include "common.hpp"

/**
 * @brief Generate a psuedo-random number in the range [start, end)
 * @param seed The seed
 * @param start The start of the range (inclusive)
 * @param end The end of the range (exclusive)
 * @return The random integer
 */
std::size_t random_integer(const std::size_t start, const std::size_t end);

/**
 * @brief Set the seed for the random number generator
 * @param seed The seed
 */
void set_seed(const uint32_t seed);

/**
 * Check if the only remaining characters in the input are whitespace
 * @param input The input stream
 * @return True if the only remaining characters are whitespace, false otherwise
*/
bool only_whitespace_remaining(std::istream &input);

/**
 * @brief Run Tarjan's algorithm to find the strongly connected components
 * @param graph The graph
 * @return Vector of subgraphs for each strongly connected component (Note that edges between strongly connected components are not included)
 * @note The time complexity is O(|V| * |E|)
 */
ordered_graphs_t tarjans_subgraphs(const graph_t &graph);

/**
 * @brief Detect cycles in the graph
 * @param graph The graph
 * @return True if there are cycles, false otherwise
 */
bool detect_cycles(const graph_t &graph);

/**
 * @brief Sort the map by values
 * @param map The map to sort
 * @param compare The comparison function
 * @return The keys in ascending/descending order
 */
template <typename T, typename V>
std::vector<T> mapsort(const std::unordered_map<T, V> &map, std::function<bool(const V &, const V &)> compare)
{
  // Sort the map by values
  std::vector<std::pair<T, V>> pairs(map.begin(), map.end());
  std::sort(pairs.begin(), pairs.end(), [&compare](const auto &a, const auto &b)
            { return compare(a.second, b.second); });

  // Extract the keys
  std::vector<T> keys;
  keys.reserve(pairs.size());
  for (const auto &pair : pairs)
  {
    keys.push_back(pair.first);
  }

  return keys;
}
