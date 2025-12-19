#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    mapView(nullptr),
    searchEngine(nullptr),
    router(nullptr),
    searchBox(nullptr),
    searchResults(nullptr),
    btnSetStart(nullptr),
    btnSetEnd(nullptr),
    btnFindRoute(nullptr),
    btnZoomIn(nullptr),
    btnZoomOut(nullptr),
    btnResetView(nullptr),
    statusLabel(nullptr),
    startNodeId(-1),
    endNodeId(-1)
{
    setWindowTitle("Map Navigator - Qt Project");
    resize(1200, 800);

    setupUI();

    searchEngine = new SearchEngine(this);
    router = new Router(this);

    loadSampleData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Top toolbar
    QWidget* toolbar = new QWidget(this);
    toolbar->setObjectName("toolbar");
    QHBoxLayout* searchLayout = new QHBoxLayout(toolbar);
    searchLayout->setContentsMargins(10, 10, 10, 10);

    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("🔍 Search for a place...");
    searchBox->setObjectName("searchBox");
    searchLayout->addWidget(searchBox);

    btnSetStart = new QPushButton("📍 Set Start", this);
    btnSetEnd = new QPushButton("🎯 Set End", this);
    btnFindRoute = new QPushButton("🚗 Find Route", this);

    btnSetStart->setObjectName("btnStart");
    btnSetEnd->setObjectName("btnEnd");
    btnFindRoute->setObjectName("btnRoute");

    searchLayout->addWidget(btnSetStart);
    searchLayout->addWidget(btnSetEnd);
    searchLayout->addWidget(btnFindRoute);

    mainLayout->addWidget(toolbar);

    // Search results
    searchResults = new QListWidget(this);
    searchResults->setMaximumHeight(150);
    searchResults->setObjectName("searchResults");
    searchResults->hide();
    mainLayout->addWidget(searchResults);

    // Map view with zoom controls
    QWidget* mapContainer = new QWidget(this);
    QVBoxLayout* mapLayout = new QVBoxLayout(mapContainer);
    mapLayout->setContentsMargins(0, 0, 0, 0);

    mapView = new MapView(this);
    mapLayout->addWidget(mapView);

    // Zoom controls overlay
    QWidget* zoomControls = new QWidget(mapView);
    zoomControls->setObjectName("zoomControls");
    QVBoxLayout* zoomLayout = new QVBoxLayout(zoomControls);
    zoomLayout->setSpacing(5);
    zoomLayout->setContentsMargins(5, 5, 5, 5);

    btnZoomIn = new QPushButton("+", zoomControls);
    btnZoomOut = new QPushButton("-", zoomControls);
    btnResetView = new QPushButton("⌖", zoomControls);

    btnZoomIn->setObjectName("zoomBtn");
    btnZoomOut->setObjectName("zoomBtn");
    btnResetView->setObjectName("zoomBtn");

    btnZoomIn->setFixedSize(40, 40);
    btnZoomOut->setFixedSize(40, 40);
    btnResetView->setFixedSize(40, 40);

    zoomLayout->addWidget(btnZoomIn);
    zoomLayout->addWidget(btnZoomOut);
    zoomLayout->addWidget(btnResetView);

    zoomControls->setGeometry(10, 10, 50, 140);

    mainLayout->addWidget(mapContainer, 1);

    // Status bar
    statusLabel = new QLabel("Ready | Drag to pan, scroll to zoom", this);
    statusLabel->setObjectName("statusLabel");
    statusBar()->addWidget(statusLabel);
    statusBar()->setStyleSheet("QStatusBar { background: #2c3e50; color: white; }");

    // Apply styles
    applyStyles();

    // Connect signals
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(searchResults, &QListWidget::itemClicked, this, &MainWindow::onSearchResultSelected);
    connect(btnSetStart, &QPushButton::clicked, this, &MainWindow::onSetStartClicked);
    connect(btnSetEnd, &QPushButton::clicked, this, &MainWindow::onSetEndClicked);
    connect(btnFindRoute, &QPushButton::clicked, this, &MainWindow::onFindRouteClicked);
    connect(btnZoomIn, &QPushButton::clicked, this, &MainWindow::onZoomInClicked);
    connect(btnZoomOut, &QPushButton::clicked, this, &MainWindow::onZoomOutClicked);
    connect(btnResetView, &QPushButton::clicked, this, &MainWindow::onResetViewClicked);
}

void MainWindow::applyStyles()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #ecf0f1;
        }

        #toolbar {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #3498db, stop:1 #2980b9);
            border-bottom: 2px solid #2574a9;
        }

        #searchBox {
            padding: 10px 15px;
            font-size: 14px;
            border: 2px solid #bdc3c7;
            border-radius: 20px;
            background-color: white;
            color: #2c3e50;
            min-width: 300px;
        }

        #searchBox:focus {
            border: 2px solid #3498db;
            outline: none;
            background-color: white;
            color: #2c3e50;
        }

        QPushButton {
            padding: 10px 20px;
            font-size: 13px;
            font-weight: bold;
            border: none;
            border-radius: 5px;
            color: white;
            min-width: 100px;
        }

        #btnStart {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #27ae60, stop:1 #229954);
        }

        #btnStart:hover {
            background: #27ae60;
        }

        #btnStart:pressed {
            background: #1e8449;
        }

        #btnEnd {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #e74c3c, stop:1 #c0392b);
        }

        #btnEnd:hover {
            background: #e74c3c;
        }

        #btnEnd:pressed {
            background: #c0392b;
        }

        #btnRoute {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #f39c12, stop:1 #e67e22);
        }

        #btnRoute:hover {
            background: #f39c12;
        }

        #btnRoute:pressed {
            background: #d68910;
        }

        #zoomControls {
            background-color: rgba(255, 255, 255, 230);
            border-radius: 8px;
            border: 1px solid #bdc3c7;
        }

        #zoomBtn {
            background-color: white;
            color: #2c3e50;
            font-size: 20px;
            font-weight: bold;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            padding: 0px;
            min-width: 40px;
        }

        #zoomBtn:hover {
            background-color: #3498db;
            color: white;
            border: 1px solid #2980b9;
        }

        #zoomBtn:pressed {
            background-color: #2980b9;
        }

        #searchResults {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
            margin: 0px 10px;
            font-size: 13px;
            color: #2c3e50;
        }

        #searchResults::item {
            padding: 8px;
            border-bottom: 1px solid #ecf0f1;
            color: #2c3e50;
            background-color: white;
        }

        #searchResults::item:hover {
            background-color: #3498db;
            color: white;
        }

        #searchResults::item:selected {
            background-color: #2980b9;
            color: white;
        }

        #statusLabel {
            color: white;
            padding: 5px;
            font-size: 12px;
        }
    )");
}

void MainWindow::loadSampleData()
{
    std::vector<Node> nodes;
    AdjacencyList graph;

    // Create a more realistic city layout with varied distances
    double baseLat = 28.6139;
    double baseLon = 77.2090;

    // Create nodes at realistic positions (not uniform grid)
    nodes.push_back(Node(0, baseLat + 0.040, baseLon + 0.005, "Central Park"));
    nodes.push_back(Node(1, baseLat + 0.038, baseLon + 0.015, ""));
    nodes.push_back(Node(2, baseLat + 0.035, baseLon + 0.025, ""));
    nodes.push_back(Node(3, baseLat + 0.040, baseLon + 0.032, ""));
    nodes.push_back(Node(4, baseLat + 0.042, baseLon + 0.045, "Airport"));

    nodes.push_back(Node(5, baseLat + 0.028, baseLon + 0.008, ""));
    nodes.push_back(Node(6, baseLat + 0.025, baseLon + 0.018, "City Hall"));
    nodes.push_back(Node(7, baseLat + 0.025, baseLon + 0.028, ""));
    nodes.push_back(Node(8, baseLat + 0.028, baseLon + 0.038, "Train Station"));
    nodes.push_back(Node(9, baseLat + 0.030, baseLon + 0.048, ""));

    nodes.push_back(Node(10, baseLat + 0.015, baseLon + 0.005, ""));
    nodes.push_back(Node(11, baseLat + 0.012, baseLon + 0.015, ""));
    nodes.push_back(Node(12, baseLat + 0.015, baseLon + 0.025, "Main Square"));
    nodes.push_back(Node(13, baseLat + 0.018, baseLon + 0.035, ""));
    nodes.push_back(Node(14, baseLat + 0.015, baseLon + 0.045, ""));

    nodes.push_back(Node(15, baseLat + 0.005, baseLon + 0.008, ""));
    nodes.push_back(Node(16, baseLat + 0.002, baseLon + 0.018, "Shopping Mall"));
    nodes.push_back(Node(17, baseLat + 0.005, baseLon + 0.028, ""));
    nodes.push_back(Node(18, baseLat + 0.008, baseLon + 0.038, ""));
    nodes.push_back(Node(19, baseLat + 0.005, baseLon + 0.048, ""));

    nodes.push_back(Node(20, baseLat - 0.005, baseLon + 0.005, "University"));
    nodes.push_back(Node(21, baseLat - 0.008, baseLon + 0.015, ""));
    nodes.push_back(Node(22, baseLat - 0.005, baseLon + 0.025, ""));
    nodes.push_back(Node(23, baseLat - 0.002, baseLon + 0.035, "Hospital"));
    nodes.push_back(Node(24, baseLat - 0.005, baseLon + 0.045, "Stadium"));

    // Create realistic road connections with varied distances
    // Main highways
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            int nodeId = i * 5 + j;
            int rightId = i * 5 + (j + 1);
            double dist = nodes[nodeId].coord.distanceTo(nodes[rightId].coord);
            double speed = (i == 2) ? 70.0 : 50.0; // Main square row is faster
            graph[nodeId].push_back(Edge(rightId, dist, speed, "Main Road"));
            graph[rightId].push_back(Edge(nodeId, dist, speed, "Main Road"));
        }
    }

    // Vertical connections
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            int nodeId = i * 5 + j;
            int downId = (i + 1) * 5 + j;
            double dist = nodes[nodeId].coord.distanceTo(nodes[downId].coord);
            double speed = (j == 2) ? 70.0 : 50.0; // Middle column is faster
            graph[nodeId].push_back(Edge(downId, dist, speed, "Avenue"));
            graph[downId].push_back(Edge(nodeId, dist, speed, "Avenue"));
        }
    }

    // Add some diagonal shortcuts (expressways)
    graph[0].push_back(Edge(6, nodes[0].coord.distanceTo(nodes[6].coord), 80.0, "Express Way"));
    graph[6].push_back(Edge(0, nodes[0].coord.distanceTo(nodes[6].coord), 80.0, "Express Way"));

    graph[4].push_back(Edge(8, nodes[4].coord.distanceTo(nodes[8].coord), 80.0, "Airport Express"));
    graph[8].push_back(Edge(4, nodes[4].coord.distanceTo(nodes[8].coord), 80.0, "Airport Express"));

    graph[12].push_back(Edge(18, nodes[12].coord.distanceTo(nodes[18].coord), 80.0, "Metro Line"));
    graph[18].push_back(Edge(12, nodes[12].coord.distanceTo(nodes[18].coord), 80.0, "Metro Line"));

    graph[20].push_back(Edge(24, nodes[20].coord.distanceTo(nodes[24].coord), 60.0, "Ring Road"));
    graph[24].push_back(Edge(20, nodes[20].coord.distanceTo(nodes[24].coord), 60.0, "Ring Road"));

    searchEngine->buildIndex(nodes);
    router->setGraph(graph, nodes);

    mapView->setNodes(nodes);
    mapView->setGraph(graph);
    mapView->centerOn(nodes[12].coord);

    statusLabel->setText(QString("Loaded %1 locations | Search or click on map").arg(nodes.size()));
}

void MainWindow::onSearchTextChanged(const QString& text)
{
    if (text.isEmpty()) {
        searchResults->hide();
        return;
    }

    auto results = searchEngine->search(text);
    searchResults->clear();

    for (const auto& result : results) {
        searchResults->addItem(result.second);
    }

    searchResults->setVisible(!results.empty());
}

void MainWindow::onSearchResultSelected(QListWidgetItem* item)
{
    QString placeName = item->text();
    int nodeId = searchEngine->getNodeId(placeName);

    if (nodeId >= 0) {
        GeoCoord coord = router->getNodeCoord(nodeId);
        mapView->centerOn(coord);
        mapView->setHighlightNode(nodeId);
    }

    searchResults->hide();
    searchBox->clear();
}

void MainWindow::onSetStartClicked()
{
    int highlightedNode = mapView->getHighlightedNode();
    if (highlightedNode >= 0) {
        startNodeId = highlightedNode;
        statusLabel->setText(QString("Start set: Node %1").arg(startNodeId));
    } else {
        QMessageBox::warning(this, "No Selection", "Please search and select a location first.");
    }
}

void MainWindow::onSetEndClicked()
{
    int highlightedNode = mapView->getHighlightedNode();
    if (highlightedNode >= 0) {
        endNodeId = highlightedNode;
        statusLabel->setText(QString("End set: Node %1").arg(endNodeId));
    } else {
        QMessageBox::warning(this, "No Selection", "Please search and select a location first.");
    }
}

void MainWindow::onFindRouteClicked()
{
    if (startNodeId < 0 || endNodeId < 0) {
        QMessageBox::warning(this, "Missing Locations",
                             "Please set both start and end locations.");
        return;
    }

    auto route = router->findRoute(startNodeId, endNodeId);

    if (route.empty()) {
        QMessageBox::information(this, "No Route", "Could not find a route.");
        return;
    }

    mapView->setRoute(route);

    // Calculate total distance
    double totalDist = 0;
    for (const auto& step : route) {
        totalDist += step.distance;
    }

    // Calculate time: 5 minutes per km
    double totalTime = (totalDist / 1000.0) * 5.0;

    // Format time display
    QString timeStr;
    if (totalTime < 60.0) {
        timeStr = QString("%1 mins").arg(static_cast<int>(totalTime));
    } else {
        int hours = static_cast<int>(totalTime / 60.0);
        int mins = static_cast<int>(totalTime) % 60;
        timeStr = QString("%1h %2m").arg(hours).arg(mins);
    }

    // Show distance and time (removed steps)
    statusLabel->setText(QString("Route found: %1 km | %2")
                             .arg(totalDist / 1000.0, 0, 'f', 2)
                             .arg(timeStr));
}

void MainWindow::onZoomInClicked()
{
    mapView->zoomIn();
}

void MainWindow::onZoomOutClicked()
{
    mapView->zoomOut();
}

void MainWindow::onResetViewClicked()
{
    if (!router) return;

    // Reset to center node (Main Square)
    GeoCoord center = router->getNodeCoord(12);
    mapView->centerOn(center);
    mapView->resetZoom();
    statusLabel->setText("View reset to Main Square");
}
