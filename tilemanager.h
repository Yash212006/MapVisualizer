#ifndef TILEMANAGER_H
#define TILEMANAGER_H

#include <QObject>

class TileManager : public QObject {
    Q_OBJECT

public:
    explicit TileManager(QObject *parent = nullptr);
};

#endif // TILEMANAGER_H
