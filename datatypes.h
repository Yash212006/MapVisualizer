#ifndef DATATYPES_H
#define DATATYPES_H

#include <QString>
#include <QPointF>
#include <vector>
#include <unordered_map>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Geographic coordinate
struct GeoCoord {
    double lat;
    double lon;

    GeoCoord(double la = 0.0, double lo = 0.0) : lat(la), lon(lo) {}

    // Haversine distance in meters
    double distanceTo(const GeoCoord& other) const {
        const double R = 6371000.0; // Earth radius in meters
        double lat1 = lat * M_PI / 180.0;
        double lat2 = other.lat * M_PI / 180.0;
        double dLat = (other.lat - lat) * M_PI / 180.0;
        double dLon = (other.lon - lon) * M_PI / 180.0;

        double a = sin(dLat/2.0) * sin(dLat/2.0) +
                   cos(lat1) * cos(lat2) *
                       sin(dLon/2.0) * sin(dLon/2.0);
        double c = 2.0 * atan2(sqrt(a), sqrt(1.0-a));
        return R * c;
    }
};

// Road network node
struct Node {
    int id;
    GeoCoord coord;
    QString name;

    Node(int i = -1, double lat = 0.0, double lon = 0.0, const QString& n = QString())
        : id(i), coord(lat, lon), name(n) {}
};

// Road network edge
struct Edge {
    int toNode;
    double distance;
    double speed;
    QString roadName;

    Edge(int to, double dist, double spd = 50.0, const QString& name = QString())
        : toNode(to), distance(dist), speed(spd), roadName(name) {}

    double travelTime() const {
        return distance / (speed * 1000.0 / 3600.0);
    }
};

// Graph structure
using AdjacencyList = std::unordered_map<int, std::vector<Edge>>;

// Route step
struct RouteStep {
    QString instruction;
    double distance;
    GeoCoord location;
};

// Tile coordinates
struct TileCoord {
    int x, y, zoom;

    TileCoord(int xx = 0, int yy = 0, int z = 10)
        : x(xx), y(yy), zoom(z) {}

    bool operator==(const TileCoord& other) const {
        return x == other.x && y == other.y && zoom == other.zoom;
    }
};

// Hash function for TileCoord
namespace std {
template<>
struct hash<TileCoord> {
    size_t operator()(const TileCoord& t) const {
        return ((static_cast<size_t>(t.zoom) << 20) |
                (static_cast<size_t>(t.x) << 10) |
                static_cast<size_t>(t.y));
    }
};
}

// Trie node for search
struct TrieNode {
    std::unordered_map<QChar, TrieNode*> children;
    bool isEndOfWord;
    int nodeId;

    TrieNode() : isEndOfWord(false), nodeId(-1) {}

    ~TrieNode() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

#endif // DATATYPES_H
