#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <QObject>
#include <vector>
#include "datatypes.h"

class SearchEngine : public QObject {
    Q_OBJECT

public:
    explicit SearchEngine(QObject *parent = nullptr);
    ~SearchEngine();

    void buildIndex(const std::vector<Node>& nodes);
    std::vector<std::pair<int, QString>> search(const QString& prefix, int maxResults = 10);
    int getNodeId(const QString& name);

private:
    void insert(const QString& word, int nodeId);
    void findAllWords(TrieNode* node, QString prefix,
                      std::vector<std::pair<int, QString>>& results, int maxResults);

    TrieNode* root;
    std::unordered_map<QString, int> nameToId;
};

#endif // SEARCHENGINE_H
