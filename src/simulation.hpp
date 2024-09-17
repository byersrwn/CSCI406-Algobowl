#pragma once

#include "common.hpp"

/**
 * @brief Run the automaton simulation
 * @param component The strongly connected component
 * @param agents The number of agents
 * @param steps The number of steps
 * @param batches The maximum number of batches (The number of steps per agent to simulate between normalized traffic change checks)
 * @param change_threshold The normalized traffic change threshold (If the change in the normalized traffic between batches falls below this threshold, terminate the simulation early)
 * @return The traffic map
 */
unnormalized_vertex_traffic_map_t simulate(const graph_t &component, const std::size_t agents, const std::size_t steps, const std::size_t batches, const double change_threshold);
