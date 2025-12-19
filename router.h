#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <vector>
#include <queue>
#include "datatypes.h"

class Router : public QObject {
    Q_OBJECT

public:
    explicit Router(QObject *parent = nullptr);

    void setGraph(const AdjacencyList& g, const std::vector<Node>& n);
    std::vector<RouteStep> findRoute(int startNodeId, int endNodeId);
    GeoCoord getNodeCoord(int nodeId) const;

private:
    struct State {
        int nodeId;
        double cost;

        bool operator>(const State& other) const {
            return cost > other.cost;
        }
    };

    AdjacencyList graph;
    std::vector<Node> nodes;
};

#endif // ROUTER_H
