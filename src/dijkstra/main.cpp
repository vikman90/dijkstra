/**
 * @file main.cpp
 * @brief Command-line interface for running Dijkstra's algorithm.
 */

#include "dijkstra/dijkstra.h"
#include "dijkstra/exceptions.h"
#include "dijkstra/graph.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

void print_help(std::string_view program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS] [FILE]\n\n"
              << "Compute shortest paths in a weighted graph using Dijkstra's algorithm.\n"
              << "If FILE is omitted or '-' is specified, reads the graph from standard input.\n\n"
              << "Options:\n"
              << "  -s, --start <NODE>    Starting source node index (default: 0)\n"
              << "  -t, --target <NODE>   Target destination node index (enables early exit)\n"
              << "  -f, --format <FORMAT> Output format: 'text', 'dot', 'json' (default: text)\n"
              << "  -b, --benchmark       Measure and print computation time on stderr\n"
              << "  -q, --quiet           Suppress informational messages\n"
              << "  -h, --help            Display this help message and exit\n\n"
              << "Examples:\n"
              << "  " << program_name << " graph.txt\n"
              << "  " << program_name << " -s 0 -t 5 -f text graph.txt\n"
              << "  random-graph -n 10 -k 3 | " << program_name << " -s 0\n";
}

void output_json(const dijkstra::DijkstraResult &result, const dijkstra::Graph &graph) {
    std::cout << "{\n"
              << "  \"source\": " << result.source() << ",\n"
              << "  \"nodes\": " << graph.node_count() << ",\n"
              << "  \"edges\": " << graph.edge_count() << ",\n"
              << "  \"connected\": " << (result.is_connected() ? "true" : "false") << ",\n"
              << "  \"results\": [\n";

    for (dijkstra::NodeId i = 0; i < graph.node_count(); ++i) {
        std::cout << "    {\n"
                  << "      \"node\": " << i << ",\n"
                  << "      \"reachable\": " << (result.has_path_to(i) ? "true" : "false") << ",\n";

        if (result.has_path_to(i)) {
            std::cout << "      \"distance\": " << *result.distance_to(i) << ",\n"
                      << "      \"path\": [";
            const auto path = result.path_to(i);
            if (path) {
                for (std::size_t p = 0; p < path->size(); ++p) {
                    std::cout << (*path)[p] << (p + 1 < path->size() ? ", " : "");
                }
            }
            std::cout << "]\n";
        } else {
            std::cout << "      \"distance\": null,\n"
                      << "      \"path\": null\n";
        }

        std::cout << "    }" << (i + 1 < graph.node_count() ? "," : "") << "\n";
    }
    std::cout << "  ]\n}\n";
}

} // namespace

int main(int argc, char **argv) {
    dijkstra::NodeId start_node = 0;
    std::optional<dijkstra::NodeId> target_node = std::nullopt;
    std::string format = "text";
    std::string input_filepath;
    bool benchmark = false;
    bool quiet = false;

    // Command-line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        } else if (arg == "-s" || arg == "--start") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            start_node = std::stoul(argv[++i]);
        } else if (arg == "-t" || arg == "--target") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            target_node = std::stoul(argv[++i]);
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            format = argv[++i];
        } else if (arg == "-b" || arg == "--benchmark") {
            benchmark = true;
        } else if (arg == "-q" || arg == "--quiet") {
            quiet = true;
        } else if (arg.rfind("-", 0) == 0 && arg != "-") {
            std::cerr << "Error: Unknown option '" << arg << "'. Use --help for usage.\n";
            return EXIT_FAILURE;
        } else {
            input_filepath = arg;
        }
    }

    try {
        dijkstra::Graph graph;

        if (!input_filepath.empty() && input_filepath != "-") {
            std::ifstream file(input_filepath);
            if (!file.is_open()) {
                std::cerr << "Error: Cannot open input file '" << input_filepath << "'.\n";
                return EXIT_FAILURE;
            }
            file >> graph;
        } else {
            std::cin >> graph;
        }

        if (graph.node_count() == 0) {
            std::cerr << "Error: Graph is empty or no valid graph data was provided.\n";
            return EXIT_FAILURE;
        }

        if (!quiet) {
            std::cerr << "Running Dijkstra's algorithm... ";
        }

        const auto t_start = std::chrono::high_resolution_clock::now();

        dijkstra::DijkstraResult result = target_node.has_value()
            ? dijkstra::shortest_path(graph, start_node, *target_node)
            : dijkstra::shortest_paths(graph, start_node);

        const auto t_end = std::chrono::high_resolution_clock::now();
        const auto duration_us = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();

        if (!quiet || benchmark) {
            std::cerr << "[" << (static_cast<double>(duration_us) / 1000.0) << " ms.]\n";
        }

        // Output results according to requested format
        if (format == "json") {
            output_json(result, graph);
        } else if (format == "dot") {
            std::cout << graph.to_dot("DijkstraResult");
        } else {
            if (target_node.has_value()) {
                if (result.has_path_to(*target_node)) {
                    std::cout << "Shortest path from " << start_node << " to " << *target_node
                              << " (distance: " << *result.distance_to(*target_node) << "):\n";
                    const auto path = *result.path_to(*target_node);
                    for (std::size_t i = 0; i < path.size(); ++i) {
                        std::cout << path[i] << (i + 1 < path.size() ? " → " : "\n");
                    }
                } else {
                    std::cout << "No path found from " << start_node << " to " << *target_node << ".\n";
                }
            } else {
                result.print_summary(std::cout);
                if (!result.is_connected() && !quiet) {
                    std::cerr << "WARN: The graph is not fully connected.\n";
                }
            }
        }

        return EXIT_SUCCESS;
    } catch (const dijkstra::DijkstraException &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
