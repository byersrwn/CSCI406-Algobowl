#include <chrono>
#include <fstream>
#include <iostream>
#include <unordered_map>

#include "boost/graph/adjacency_list.hpp"
#include "boost/program_options.hpp"
#include "helpers.hpp"
#include "input.hpp"
#include "output.hpp"
#include "simulation.hpp"

/**
 * @brief The stide between progress updates for vertex processing
 */
#define VERTEX_PROCESSING_PROGRESS_STRIDE 250

/**
 * @brief Compare two integers such that they are sorted in ascending order
 * @param a The first integer
 * @param b The second integer
 */
static inline bool trafficCompare(std::size_t a, std::size_t b)
{
  return a < b;
}

int main(int argc, char *argv[])
{
  // Arguments
  boost::program_options::positional_options_description positional;
  positional.add("input", 1);
  positional.add("output", 1);

  // Options
  boost::program_options::options_description description("Allowed options");
  description.add_options()                                                                                                                                                                                                                           // Force wrap
      ("input", boost::program_options::value<std::string>(), "Input file")                                                                                                                                                                           // Force wrap
      ("output", boost::program_options::value<std::string>(), "Output file")                                                                                                                                                                         // Force wrap
      ("agents", boost::program_options::value<std::size_t>()->default_value(1000), "Number of agents")                                                                                                                                               // Force wrap
      ("steps", boost::program_options::value<std::size_t>()->default_value(1000), "Number of steps")                                                                                                                                                 // Force wrap
      ("batches", boost::program_options::value<std::size_t>()->default_value(250), "Maximum number of batches (Number of steps per agent to simulate between normalized traffic change checks)")                                                     // Force wrap                                                                    // Force wrap
      ("change-threshold", boost::program_options::value<double>()->default_value(0.001), "Normalized traffic change threshold (If the change in the normalized traffic between batches falls below this threshold, terminate the simulation early)") // Force wrap
      ("help", "Print this help message");

  // Parse the arguments and options
  boost::program_options::variables_map options;
  boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).positional(positional).run(), options);
  boost::program_options::notify(options);

  // Print help
  if (options.contains("help"))
  {
    std::cout << description << std::endl;
    return 1;
  }
  // Validate the input and output
  else if (!options.contains("input") || !options.contains("output"))
  {
    std::cerr << "Error: input and output files are required" << std::endl;
    return 1;
  }

  // Get the options
  std::string inputFilename = options["input"].as<std::string>();
  std::string outputFilename = options["output"].as<std::string>();
  std::size_t agents = options["agents"].as<std::size_t>();
  std::size_t steps = options["steps"].as<std::size_t>();
  std::size_t batches = options["batches"].as<std::size_t>();
  double changeThreshold = options["change-threshold"].as<double>();

  // Get the time
  auto startTime = std::chrono::steady_clock::now();

  // Open the files
  std::ifstream input;
  input.open(inputFilename);

  if (!input.is_open())
  {
    std::cerr << "Error: failed to open input file: " << inputFilename << std::endl;
    return 1;
  }

  std::ofstream output;
  output.open(outputFilename);

  if (!output.is_open())
  {
    std::cerr << "Error: failed to open output file:" << outputFilename << std::endl;
    return 1;
  }

  // Deserialize the input
  auto graph = deserialize_input(input);

  // Run Tarjans
  const auto subgraphs = tarjans_subgraphs(graph);

  // Get vertices to remove
  unordered_vertex_properties_t cutVertices;
  std::size_t subgraphsSize = subgraphs.size();
  std::size_t subgraphIndex = 0;

  for (const auto &subgraph : subgraphs)
  {
    std::size_t subgraphVertices = boost::num_vertices(subgraph);

    // Skip if the subgraph is just a single vertex
    if (subgraphVertices == 1)
    {
      continue;
    }

    // Run the simulation
    const auto traffic = simulate(subgraph, agents, steps, batches, changeThreshold);

    // Sort by traffic
    const auto sorted = mapsort<vertex_descriptor_t, std::size_t>(traffic, trafficCompare);

    // Build the acyclic graph vertex by vertex
    graph_t acyclicGraph;
    std::unordered_map<vertex_properties_s, vertex_descriptor_t> acyclicVertices;
    std::size_t vertexProgressIndex = 0;

    for (const auto &subgraphVertexDescriptor : sorted)
    {
      // Get the vertex
      const auto vertexProperties = subgraph[subgraphVertexDescriptor];

      // Add the vertex
      const auto cloneVertexDescriptor = boost::add_vertex(vertexProperties, acyclicGraph);
      acyclicVertices[vertexProperties] = cloneVertexDescriptor;

      // Add in edges
      for (const auto &edgeDescriptor : boost::make_iterator_range(boost::in_edges(subgraphVertexDescriptor, subgraph)))
      {
        // Get the source vertex properties
        const auto sourceProperties = subgraph[boost::source(edgeDescriptor, subgraph)];

        // Skip if the source vertex is not in the acyclic graph
        if (!acyclicVertices.contains(sourceProperties))
        {
          continue;
        }

        // Get the source vertex descriptor
        const auto cloneSourceDescriptor = acyclicVertices[sourceProperties];

        // Add the edge
        boost::add_edge(cloneSourceDescriptor, cloneVertexDescriptor, acyclicGraph);
      }

      // Add out edges
      for (const auto &edgeDescriptor : boost::make_iterator_range(boost::out_edges(subgraphVertexDescriptor, subgraph)))
      {
        // Get the target vertex properties
        const auto targetProperties = subgraph[boost::target(edgeDescriptor, subgraph)];

        // Skip if the target vertex is not in the acyclic graph
        if (!acyclicVertices.contains(targetProperties))
        {
          continue;
        }

        // Get the target vertex descriptor
        const auto cloneTargetDescriptor = acyclicVertices[targetProperties];

        // Add the edge
        boost::add_edge(cloneVertexDescriptor, cloneTargetDescriptor, acyclicGraph);
      }

      // Revert changes if the graph is cyclic
      if (detect_cycles(acyclicGraph))
      {
        acyclicVertices.erase(vertexProperties);
        boost::clear_vertex(cloneVertexDescriptor, acyclicGraph);
        boost::remove_vertex(cloneVertexDescriptor, acyclicGraph);
      }

      // Update and print progress
      vertexProgressIndex++;
      if (vertexProgressIndex % VERTEX_PROCESSING_PROGRESS_STRIDE == 0)
      {
        std::cout << "Processed vertex " << vertexProgressIndex << " of " << sorted.size() << " (" << vertexProgressIndex * 100 / sorted.size() << "%)" << std::endl;
      }
    }

    // Add all vertices to remove which are not in the acyclic graph
    for (const auto &vertexDescriptor : sorted)
    {
      if (acyclicVertices.find(subgraph[vertexDescriptor]) == acyclicVertices.end())
      {
        cutVertices.insert(subgraph[vertexDescriptor]);
      }
    }

    // Update and print progress
    subgraphIndex++;
    std::cout << "Processed component " << subgraphIndex << " of " << subgraphsSize << " (" << subgraphIndex * 100 / subgraphsSize << "%)" << std::endl;
  }

  // Serialize the output
  serialize_output(output, cutVertices);

  // Get the time
  auto endTime = std::chrono::steady_clock::now();

  // Print the elapsed time
  std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s" << std::endl;

  return 0;
}
