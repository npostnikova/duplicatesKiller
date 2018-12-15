#ifndef TRIE_H
#define TRIE_H

#include <QByteArray>
#include <QLibrary>
#include <vector>
#include <cstdlib>

using namespace std;

struct Node {
    size_t fileIndex;
    size_t l, len;

    std::vector<size_t> term;
    std::vector<Node*> values;

    Node(size_t ind, size_t l, size_t r) : fileIndex(ind), l(l), len(r),
                                                         term({ind}), values(CHARSET, nullptr) {}

private:
    Node() : fileIndex(0), l(0), len(0), values(CHARSET, nullptr) {}

    friend struct Trie;
    static const size_t CHARSET = 256;
};

struct Trie {
    Trie(std::vector<QByteArray> const& files);

    void build();

    void insert(size_t index);

    std::vector<vector<size_t>> getDuplicates();

private:
    Node * newNode(size_t ind, size_t l, size_t r);

    void duplicatesCrawl(Node * curNode, std::vector<vector<size_t>>& result);

    inline size_t fileAt(size_t i, size_t j);

    Node root;
    std::vector<Node> data;
    std::vector<QByteArray> const& files;

    friend struct Node;
    static const size_t CHARSET = 256;
};

#endif // TRIE_H
