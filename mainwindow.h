#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include "mapview.h"
#include "searchengine.h"
#include "router.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchTextChanged(const QString& text);
    void onSearchResultSelected(QListWidgetItem* item);
    void onSetStartClicked();
    void onSetEndClicked();
    void onFindRouteClicked();
    void onZoomInClicked();
    void onZoomOutClicked();
    void onResetViewClicked();

private:
    void setupUI();
    void loadSampleData();
    void applyStyles();

    MapView* mapView;
    SearchEngine* searchEngine;
    Router* router;

    QLineEdit* searchBox;
    QListWidget* searchResults;
    QPushButton* btnSetStart;
    QPushButton* btnSetEnd;
    QPushButton* btnFindRoute;
    QPushButton* btnZoomIn;
    QPushButton* btnZoomOut;
    QPushButton* btnResetView;
    QLabel* statusLabel;

    int startNodeId;
    int endNodeId;
};

#endif // MAINWINDOW_H
