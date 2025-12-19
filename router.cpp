#include "router.h"
#include <unordered_map>
#include <limits>
#include <algorithm>

Router::Router(QObject *parent)
    : QObject(parent)
{
}

void Router::setGraph(const AdjacencyList& g, const std::vector<Node>& n)
{
    graph = g;
    nodes = n;
}

GeoCoord Router::getNodeCoord(int nodeId) const
{
    if (nodeId >= 0 && nodeId < static_cast<int>(nodes.size())) {
        return nodes[nodeId].coord;
    }
    return GeoCoord();
}

std::vector<RouteStep> Router::findRoute(int startNodeId, int endNodeId)
{
    std::vector<RouteStep> route;

    if (graph.find(startNodeId) == graph.end() ||
        graph.find(endNodeId) == graph.end()) {
        return route;
    }

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::unordered_map<int, double> distances;
    std::unordered_map<int, int> previous;

    for (const auto& node : nodes) {
        distances[node.id] = std::numeric_limits<double>::infinity();
    }
    distances[startNodeId] = 0.0;

    pq.push({startNodeId, 0.0});

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();

        if (current.nodeId == endNodeId) {
            break;
        }

        if (current.cost > distances[current.nodeId]) {
            continue;
        }

        if (graph.find(current.nodeId) != graph.end()) {
            for (const auto& edge : graph.at(current.nodeId)) {
                double newCost = current.cost + edge.distance;

                if (newCost < distances[edge.toNode]) {
                    distances[edge.toNode] = newCost;
                    previous[edge.toNode] = current.nodeId;
                    pq.push({edge.toNode, newCost});
                }
            }
        }
    }

    if (previous.find(endNodeId) == previous.end()) {
        return route;
    }

    std::vector<int> path;
    int current = endNodeId;
    while (current != startNodeId) {
        path.push_back(current);
        current = previous[current];
    }
    path.push_back(startNodeId);
    std::reverse(path.begin(), path.end());

    for (size_t i = 0; i < path.size(); i++) {
        RouteStep step;
        step.location = nodes[path[i]].coord;

        if (i == 0) {
            step.instruction = "Start at " + nodes[path[i]].name;
            step.distance = 0;
        } else {
            double dist = nodes[path[i-1]].coord.distanceTo(nodes[path[i]].coord);
            step.distance = dist;
            step.instruction = QString("Continue to %1 (%2 m)")
                                   .arg(nodes[path[i]].name).arg(static_cast<int>(dist));
        }

        route.push_back(step);
    }

    return route;
}
