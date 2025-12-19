#include "searchengine.h"

SearchEngine::SearchEngine(QObject *parent)
    : QObject(parent), root(new TrieNode())
{
}

SearchEngine::~SearchEngine()
{
    delete root;
}

void SearchEngine::buildIndex(const std::vector<Node>& nodes)
{
    for (const auto& node : nodes) {
        if (!node.name.isEmpty()) {
            insert(node.name.toLower(), node.id);
            nameToId[node.name] = node.id;
        }
    }
}

void SearchEngine::insert(const QString& word, int nodeId)
{
    TrieNode* current = root;

    for (const QChar& ch : word) {
        if (current->children.find(ch) == current->children.end()) {
            current->children[ch] = new TrieNode();
        }
        current = current->children[ch];
    }

    current->isEndOfWord = true;
    current->nodeId = nodeId;
}

std::vector<std::pair<int, QString>> SearchEngine::search(const QString& prefix, int maxResults)
{
    std::vector<std::pair<int, QString>> results;

    if (prefix.isEmpty()) {
        return results;
    }

    QString lowerPrefix = prefix.toLower();
    TrieNode* current = root;

    for (const QChar& ch : lowerPrefix) {
        if (current->children.find(ch) == current->children.end()) {
            return results;
        }
        current = current->children[ch];
    }

    findAllWords(current, lowerPrefix, results, maxResults);

    return results;
}

void SearchEngine::findAllWords(TrieNode* node, QString prefix,
                                std::vector<std::pair<int, QString>>& results,
                                int maxResults)
{
    if (results.size() >= static_cast<size_t>(maxResults)) {
        return;
    }

    if (node->isEndOfWord) {
        for (const auto& pair : nameToId) {
            if (pair.first.toLower() == prefix) {
                results.push_back({node->nodeId, pair.first});
                break;
            }
        }
    }

    for (auto& pair : node->children) {
        findAllWords(pair.second, prefix + pair.first, results, maxResults);
    }
}

int SearchEngine::getNodeId(const QString& name)
{
    auto it = nameToId.find(name);
    if (it != nameToId.end()) {
        return it->second;
    }
    return -1;
}
