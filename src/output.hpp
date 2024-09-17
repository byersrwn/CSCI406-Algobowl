#pragma once

#include <fstream>
#include <unordered_set>

#include "common.hpp"

/**
 * @brief Deserialize the input file
 * @param input The input stream
 * @return The deserialized vertices
 */
unordered_vertex_properties_t deserialize_output(std::istream &input);

/**
 * @brief Serialize the vertices
 * @param output The output stream
 * @param vertices The serialized vertices
 */
void serialize_output(std::ostream &output, const unordered_vertex_properties_t &vertices);
