/**
 * @file main.cpp
 * @brief Command-line interface for generating random graphs.
 */

#include "dijkstra/exceptions.h"
#include "dijkstra/graph.h"

#include <chrono>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace {

void print_help(std::string_view program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS] [SIZE [CONNECTIONS]]\n\n"
              << "Generate random weighted graphs for testing and benchmarking.\n\n"
              << "Options:\n"
              << "  -n, --nodes <N>         Number of vertices (default: 100)\n"
              << "  -k, --connections <K>   Target connections per node in geometric model (default: 3)\n"
              << "  -p, --prob <PROB>       Edge probability for Erdős-Rényi model (0.0 to 1.0)\n"
              << "  -s, --seed <SEED>       Random seed for deterministic generation\n"
              << "  -f, --format <FORMAT>   Output format: 'matrix', 'dot' (default: matrix)\n"
              << "  -q, --quiet             Suppress informational messages\n"
              << "  -h, --help              Display this help message and exit\n\n"
              << "Positional arguments:\n"
              << "  SIZE                    Number of nodes (overrides -n)\n"
              << "  CONNECTIONS             Connections per node (overrides -k)\n\n"
              << "Examples:\n"
              << "  " << program_name << " 5\n"
              << "  " << program_name << " -n 50 -k 4 -s 42\n"
              << "  " << program_name << " -n 20 -p 0.3 -f dot\n";
}

} // namespace

int main(int argc, char **argv) {
    std::size_t size = 100;
    std::size_t connections = 3;
    std::optional<double> erdos_prob = std::nullopt;
    std::optional<std::uint64_t> seed = std::nullopt;
    std::string format = "matrix";
    bool quiet = false;

    int positional_idx = 0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return EXIT_SUCCESS;
        } else if (arg == "-n" || arg == "--nodes") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            size = std::stoul(argv[++i]);
        } else if (arg == "-k" || arg == "--connections") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            connections = std::stoul(argv[++i]);
        } else if (arg == "-p" || arg == "--prob") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            erdos_prob = std::stod(argv[++i]);
        } else if (arg == "-s" || arg == "--seed") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            seed = std::stoull(argv[++i]);
        } else if (arg == "-f" || arg == "--format") {
            if (i + 1 >= argc) {
                std::cerr << "Error: Missing argument for " << arg << "\n";
                return EXIT_FAILURE;
            }
            format = argv[++i];
        } else if (arg == "-q" || arg == "--quiet") {
            quiet = true;
        } else if (arg.rfind("-", 0) == 0) {
            std::cerr << "Error: Unknown option '" << arg << "'. Use --help for usage.\n";
            return EXIT_FAILURE;
        } else {
            if (positional_idx == 0) {
                size = std::stoul(arg);
                positional_idx++;
            } else if (positional_idx == 1) {
                connections = std::stoul(arg);
                positional_idx++;
            } else {
                std::cerr << "Error: Unexpected argument '" << arg << "'.\n";
                return EXIT_FAILURE;
            }
        }
    }

    try {
        if (!quiet) {
            std::cerr << "Generating a random graph...    ";
        }

        const auto t_start = std::chrono::high_resolution_clock::now();

        dijkstra::Graph graph = erdos_prob.has_value()
            ? dijkstra::Graph::random_erdos_renyi(size, *erdos_prob, 1.0, 10.0, seed)
            : dijkstra::Graph::random_geometric(size, connections, seed);

        const auto t_end = std::chrono::high_resolution_clock::now();
        const auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();

        if (!quiet) {
            std::cerr << "[" << duration_ms << " ms.]\n";
        }

        if (format == "dot") {
            std::cout << graph.to_dot("RandomGraph");
        } else {
            std::cout << graph;
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
