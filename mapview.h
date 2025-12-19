#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QPoint>
#include "datatypes.h"

class MapView : public QWidget {
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);

    void centerOn(const GeoCoord& coord);
    void setNodes(const std::vector<Node>& nodes);
    void setGraph(const AdjacencyList& graph);
    void setRoute(const std::vector<RouteStep>& route);
    void setHighlightNode(int nodeId);
    int getHighlightedNode() const { return highlightedNode; }

    void zoomIn();
    void zoomOut();
    void resetZoom();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    QPointF geoToScreen(const GeoCoord& coord) const;
    GeoCoord screenToGeo(const QPointF& point) const;

    GeoCoord centerCoord;
    int zoomLevel;
    double scale;

    QPoint lastMousePos;
    bool isPanning;

    std::vector<Node> nodes;
    std::vector<RouteStep> route;
    AdjacencyList graph;
    int highlightedNode;
};

#endif // MAPVIEW_H
