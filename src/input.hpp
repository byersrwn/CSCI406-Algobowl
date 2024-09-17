#pragma once

#include <fstream>

#include "common.hpp"

/**
 * @brief Deserialize the input file
 * @param input The input stream
 * @return The deserialized graph
 */
graph_t deserialize_input(std::istream &input);

/**
 * @brief Serialize the graph
 * @param output The output stream
 * @param graph The serialized graph
 */
void serialize_input(std::ostream &output, const graph_t &graph);
