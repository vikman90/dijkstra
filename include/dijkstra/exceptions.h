/**
 * @file exceptions.h
 * @brief Domain-specific exception hierarchy for the Dijkstra library.
 */

#pragma once

#include <stdexcept>
#include <string>

namespace dijkstra {

/**
 * @brief Base exception for all Dijkstra library errors.
 */
class DijkstraException : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/**
 * @brief Exception thrown when graph serialization / parsing fails.
 */
class GraphParseException : public DijkstraException {
  public:
    using DijkstraException::DijkstraException;
};

/**
 * @brief Exception thrown when accessing an invalid or out-of-bounds node index.
 */
class InvalidNodeException : public DijkstraException {
  public:
    using DijkstraException::DijkstraException;
};

/**
 * @brief Exception thrown when an edge weight is negative (unsupported by Dijkstra).
 */
class NegativeWeightException : public DijkstraException {
  public:
    using DijkstraException::DijkstraException;
};

} // namespace dijkstra
