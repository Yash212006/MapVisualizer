#include "mapview.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <cmath>

MapView::MapView(QWidget *parent)
    : QWidget(parent),
    centerCoord(28.6139, 77.2090),
    zoomLevel(12),
    scale(0.3),
    isPanning(false),
    highlightedNode(-1)
{
    setMinimumSize(600, 400);
    setMouseTracking(true);
}

void MapView::centerOn(const GeoCoord& coord)
{
    centerCoord = coord;
    update();
}

void MapView::setNodes(const std::vector<Node>& n)
{
    nodes = n;
    update();
}

void MapView::setGraph(const AdjacencyList& g)
{
    graph = g;
    update();
}

void MapView::setRoute(const std::vector<RouteStep>& r)
{
    route = r;
    update();
}

void MapView::setHighlightNode(int nodeId)
{
    highlightedNode = nodeId;
    update();
}

void MapView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Clean background
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(248, 249, 250));
    gradient.setColorAt(1, QColor(241, 243, 245));
    painter.fillRect(rect(), gradient);

    // Subtle grid
    painter.setPen(QPen(QColor(220, 225, 230), 1, Qt::DotLine));
    for (int i = 0; i < width(); i += 50) {
        painter.drawLine(i, 0, i, height());
    }
    for (int i = 0; i < height(); i += 50) {
        painter.drawLine(0, i, width(), i);
    }

    // Draw roads
    for (size_t idx = 0; idx < nodes.size(); ++idx) {
        const Node& node = nodes[idx];

        if (graph.find(node.id) != graph.end()) {
            for (const auto& edge : graph.at(node.id)) {
                if (edge.toNode < static_cast<int>(nodes.size())) {
                    QPointF pos1 = geoToScreen(node.coord);
                    QPointF pos2 = geoToScreen(nodes[edge.toNode].coord);

                    if (edge.speed >= 70.0) {
                        // Highways - orange
                        painter.setPen(QPen(QColor(255, 167, 38, 180), 6, Qt::SolidLine, Qt::RoundCap));
                        painter.drawLine(pos1, pos2);
                        painter.setPen(QPen(QColor(255, 193, 7), 4));
                        painter.drawLine(pos1, pos2);
                    } else {
                        // Regular roads - gray
                        painter.setPen(QPen(QColor(189, 195, 199), 5, Qt::SolidLine, Qt::RoundCap));
                        painter.drawLine(pos1, pos2);
                        painter.setPen(QPen(QColor(236, 240, 241), 3));
                        painter.drawLine(pos1, pos2);
                    }
                }
            }
        }
    }

    // Draw route
    if (!route.empty()) {
        QPainterPath routePath;
        routePath.moveTo(geoToScreen(route[0].location));

        for (size_t i = 1; i < route.size(); i++) {
            routePath.lineTo(geoToScreen(route[i].location));
        }

        // Outer glow
        painter.setPen(QPen(QColor(52, 152, 219, 100), 12, Qt::SolidLine, Qt::RoundCap));
        painter.drawPath(routePath);

        // Main route line
        painter.setPen(QPen(QColor(41, 128, 185), 6, Qt::SolidLine, Qt::RoundCap));
        painter.drawPath(routePath);

        // Inner highlight
        painter.setPen(QPen(QColor(52, 152, 219), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawPath(routePath);

        // Distance labels - Always show
        for (size_t i = 1; i < route.size(); i++) {
            QPointF pos1 = geoToScreen(route[i-1].location);
            QPointF pos2 = geoToScreen(route[i].location);
            QPointF midpoint = (pos1 + pos2) / 2.0;

            QString distText;
            if (route[i].distance >= 1000.0) {
                distText = QString("%1 km").arg(route[i].distance / 1000.0, 0, 'f', 1);
            } else {
                distText = QString("%1 m").arg(static_cast<int>(route[i].distance));
            }

            QFont distFont("Arial", 9, QFont::Bold);
            painter.setFont(distFont);
            QFontMetrics fm(distFont);
            int textWidth = fm.horizontalAdvance(distText);
            int textHeight = fm.height();

            QRectF labelRect(midpoint.x() - textWidth/2 - 6, midpoint.y() - textHeight/2 - 3,
                             textWidth + 12, textHeight + 6);

            painter.setBrush(QColor(0, 0, 0, 50));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(labelRect.adjusted(2, 2, 2, 2), 4, 4);

            painter.setBrush(QColor(255, 193, 7, 240));
            painter.setPen(QPen(QColor(243, 156, 18), 2));
            painter.drawRoundedRect(labelRect, 4, 4);

            painter.setPen(QColor(44, 62, 80));
            painter.drawText(labelRect, Qt::AlignCenter, distText);
        }

        // Route markers
        for (size_t i = 0; i < route.size(); i++) {
            QPointF pos = geoToScreen(route[i].location);

            if (i == 0) {
                painter.setBrush(QColor(39, 174, 96));
                painter.setPen(QPen(Qt::white, 3));
                painter.drawEllipse(pos, 12, 12);
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(pos.x() - 4, pos.y() + 5, "S");
            } else if (i == route.size() - 1) {
                painter.setBrush(QColor(231, 76, 60));
                painter.setPen(QPen(Qt::white, 3));
                painter.drawEllipse(pos, 12, 12);
                painter.setFont(QFont("Arial", 10, QFont::Bold));
                painter.drawText(pos.x() - 4, pos.y() + 5, "E");
            }
        }
    }

    // Draw nodes
    for (const auto& node : nodes) {
        QPointF pos = geoToScreen(node.coord);

        painter.setBrush(QColor(0, 0, 0, 40));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(pos + QPointF(2, 2), 6, 6);

        if (node.id == highlightedNode) {
            painter.setBrush(QColor(231, 76, 60));
            painter.setPen(QPen(Qt::white, 3));
            painter.drawEllipse(pos, 14, 14);
            painter.setBrush(Qt::white);
            painter.drawEllipse(pos, 5, 5);
        } else if (!node.name.isEmpty() && !node.name.contains("Junction")) {
            painter.setBrush(QColor(52, 152, 219));
            painter.setPen(QPen(Qt::white, 2));
            painter.drawEllipse(pos, 7, 7);
        } else {
            painter.setBrush(QColor(149, 165, 166));
            painter.setPen(QPen(Qt::white, 1));
            painter.drawEllipse(pos, 3, 3);
        }
    }

    // Draw labels - Always show
    if (scale > 0.1) {
        for (const auto& node : nodes) {
            if (!node.name.isEmpty() && !node.name.contains("Junction")) {
                QPointF pos = geoToScreen(node.coord);

                QFont font("Arial", 9, QFont::Bold);
                painter.setFont(font);
                QFontMetrics fm(font);
                int textWidth = fm.horizontalAdvance(node.name);
                int textHeight = fm.height();

                double offsetY = (node.id % 2 == 0) ? 18 : -24;

                QRectF labelRect(pos.x() - textWidth/2 - 6,
                                 pos.y() + offsetY - textHeight/2,
                                 textWidth + 12, textHeight + 4);

                painter.setBrush(QColor(0, 0, 0, 70));
                painter.setPen(Qt::NoPen);
                painter.drawRoundedRect(labelRect.adjusted(1, 1, 1, 1), 3, 3);

                painter.setBrush(QColor(44, 62, 80, 250));
                painter.setPen(QPen(QColor(52, 73, 94), 1));
                painter.drawRoundedRect(labelRect, 3, 3);

                painter.setPen(Qt::white);
                painter.drawText(labelRect, Qt::AlignCenter, node.name);
            }
        }
    }

    // Info panel
    QString info = QString("Zoom: %1 | %2 locations")
                       .arg(static_cast<int>(scale * 100))
                       .arg(nodes.size());

    painter.setFont(QFont("Arial", 9));
    QFontMetrics fm(painter.font());
    int infoWidth = fm.horizontalAdvance(info);
    QRectF infoRect(width() - infoWidth - 25, height() - 35, infoWidth + 15, 25);

    painter.setBrush(QColor(255, 255, 255, 240));
    painter.setPen(QPen(QColor(189, 195, 199), 1));
    painter.drawRoundedRect(infoRect, 4, 4);

    painter.setPen(QColor(52, 73, 94));
    painter.drawText(infoRect, Qt::AlignCenter, info);
}

QPointF MapView::geoToScreen(const GeoCoord& coord) const
{
    double x = (coord.lon - centerCoord.lon) * 100000.0 * scale + width() / 2.0;
    double y = (centerCoord.lat - coord.lat) * 100000.0 * scale + height() / 2.0;
    return QPointF(x, y);
}

GeoCoord MapView::screenToGeo(const QPointF& point) const
{
    double lon = (point.x() - width() / 2.0) / (100000.0 * scale) + centerCoord.lon;
    double lat = centerCoord.lat - (point.y() - height() / 2.0) / (100000.0 * scale);
    return GeoCoord(lat, lon);
}

void MapView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = true;
        lastMousePos = event->pos();
    }
}

void MapView::mouseMoveEvent(QMouseEvent* event)
{
    if (isPanning) {
        QPoint delta = event->pos() - lastMousePos;

        double latDelta = -delta.y() / (100000.0 * scale);
        double lonDelta = delta.x() / (100000.0 * scale);

        centerCoord.lat += latDelta;
        centerCoord.lon += lonDelta;

        lastMousePos = event->pos();
        update();
    }
}

void MapView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        isPanning = false;
    }
}

void MapView::wheelEvent(QWheelEvent* event)
{
    double delta = event->angleDelta().y() > 0 ? 1.2 : 0.8;
    scale *= delta;

    if (scale < 0.05) scale = 0.05;
    if (scale > 30.0) scale = 30.0;

    update();
}

void MapView::zoomIn()
{
    scale *= 1.3;
    if (scale > 30.0) scale = 30.0;
    update();
}

void MapView::zoomOut()
{
    scale *= 0.7;
    if (scale < 0.05) scale = 0.05;
    update();
}

void MapView::resetZoom()
{
    scale = 0.3;
    zoomLevel = 12;
    update();
}
