#ifndef INDEX_H
#define INDEX_H

struct IndexNode {
    int key;
    int row_id;
    IndexNode* next = nullptr;
};

class Index {
private:
    IndexNode* head = nullptr;

public:
    void insert(int key, int row_id) {
        IndexNode* node = new IndexNode{key, row_id};
        node->next = head;
        head = node;
    }

    int find(int key) {
        IndexNode* cur = head;
        while (cur) {
            if (cur->key == key) return cur->row_id;
            cur = cur->next;
        }
        return -1;
    }
};

#endif
