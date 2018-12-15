#include "trie.h"

Trie::Trie(std::vector<QByteArray> const& files) : files(files) {
    data.reserve(files.size() * 2);
}
void Trie::build() {
    for (size_t i = 0; i < files.size(); i++) {
        insert(i);
    }
}

Node * Trie::newNode(size_t ind, size_t l, size_t r) {
    data.push_back(Node(ind, l, r));
    return &data.back();
}

size_t Trie::fileAt(size_t i, size_t j) {
    return (CHARSET + static_cast<int>(files[i].at(j))) % CHARSET;
}

void Trie::insert(size_t index) {
    Node * curNode = &root;
    size_t i = 0;
    while (i < files[index].size()) {
        if (curNode->values[fileAt(index, i)] != nullptr) {
            // have the symbol
            curNode = curNode->values[fileAt(index, i)];
                while (i < curNode->len + curNode->l &&
                       fileAt(index, i) == fileAt(curNode->fileIndex, i)) {
                    i++;
                }
                if (i != curNode->len + curNode->l) {
                    // we are on the edge
                    Node * n1 = newNode(curNode->fileIndex, i, curNode->len + curNode->l - i);
                    Node * n2 = newNode(index, i, files[index].size() - i);

                    swap(n1->values, curNode->values);
                    swap(n1->term, curNode->term);
                    curNode->term.clear();

                    curNode->len = i - curNode->l;
                    curNode->values[fileAt(curNode->fileIndex, i)] = n1;
                    curNode->values[fileAt(index, i)] = n2;
                    break;
                }

            if (i == files[index].size()) {
                curNode->term.push_back(index);
            }
        } else {
            curNode->values[fileAt(index, i)] = newNode(index, i, files[index].size() - i);
            break;
        }
    }
}

std::vector<vector<size_t>> Trie::getDuplicates() {
    std::vector<vector<size_t>> result;
    duplicatesCrawl(&root, result);
    return result;
}

void Trie::duplicatesCrawl(Node * curNode, std::vector<vector<size_t>>& result) {
    size_t cnt = 0;
    for (auto v : curNode->values) {
        if (v != nullptr) {
            cnt++;
            duplicatesCrawl(v, result);
        }
    }
    if (cnt == 0 && curNode->term.size() > 1) {
        result.push_back(curNode->term);
    }
}

