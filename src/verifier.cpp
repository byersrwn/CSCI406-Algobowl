#include <chrono>
#include <fstream>
#include <iostream>

#include "boost/graph/adjacency_list.hpp"
#include "boost/program_options.hpp"
#include "helpers.hpp"
#include "input.hpp"
#include "output.hpp"

int main(int argc, char *argv[])
{
  // Arguments
  boost::program_options::positional_options_description positional;
  positional.add("input", 1);
  positional.add("output", 1);

  // Options
  boost::program_options::options_description description("Allowed options");
  description.add_options()                                                   // Force wrap
      ("input", boost::program_options::value<std::string>(), "Input file")   // Force wrap
      ("output", boost::program_options::value<std::string>(), "Output file") // Force wrap
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

  std::ifstream output;
  output.open(outputFilename);

  if (!output.is_open())
  {
    std::cerr << "Error: failed to open output file:" << outputFilename << std::endl;
    return 1;
  }

  // Deserialize the input and output
  auto graph = deserialize_input(input);
  const auto vertices = deserialize_output(output);

  // Remove vertices (See https://stackoverflow.com/a/7210986)
  vertex_iterator_t start, end, next;
  std::tie(start, end) = boost::vertices(graph);
  for (next = start; next != end; start = next)
  {
    ++next;

    if (vertices.find(graph[*start]) != vertices.end())
    {
      boost::clear_vertex(*start, graph);
      boost::remove_vertex(*start, graph);
    }
  }

  // Get the time
  auto endTime = std::chrono::steady_clock::now();

  // Print the elapsed time
  std::cout << "Elapsed time: " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s" << std::endl;

  // Detect cycles
  if (detect_cycles(graph))
  {
    std::cerr << "Cycle(s) detected" << std::endl;
    return 1;
  }
  else
  {
    std::cout << "No cycle detected" << std::endl;
    return 0;
  }
}
