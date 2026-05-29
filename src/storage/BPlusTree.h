#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include "MyVector.h"
#include <cstring>
#include <cstdlib>
#include <iostream> 

struct BPlusNode {
    bool is_leaf;
    int key_count;
    int* keys;
    int* values; // 叶子存row_id，非叶子存子节点指针(需强转)
    BPlusNode** children;
    BPlusNode* parent;
    BPlusNode* next_leaf;

    BPlusNode(bool leaf, int order) : is_leaf(leaf), key_count(0), parent(nullptr), next_leaf(nullptr) {
        keys = new int[order];
        values = new int[order];
        children = new BPlusNode*[order + 1];
        for(int i=0; i<=order; ++i) children[i] = nullptr;
    }

    ~BPlusNode() {
        delete[] keys;
        delete[] values;
        delete[] children;
    }
};

class BPlusTree {
private:
    BPlusNode* root;
    int order;

    void destroy_node(BPlusNode* node) {
        if (!node) return;
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; ++i) {
                destroy_node(node->children[i]);
            }
        }
        delete node;
    }

    BPlusNode* find_leaf(int key) {
        if (!root) return nullptr;
        BPlusNode* cur = root;
        while (!cur->is_leaf) {
            int i = 0;
            while (i < cur->key_count && key >= cur->keys[i]) i++;
            cur = cur->children[i];
        }
        return cur;
    }

    void insert_into_parent(BPlusNode* left, int key, BPlusNode* right) {
        if (!left->parent) {
            BPlusNode* new_root = new BPlusNode(false, order);
            new_root->keys[0] = key;
            new_root->children[0] = left;
            new_root->children[1] = right;
            new_root->key_count = 1;
            left->parent = new_root;
            right->parent = new_root;
            root = new_root;
            return;
        }

        BPlusNode* parent = left->parent;
        if (parent->key_count < order - 1) {
            int i = parent->key_count - 1;
            while (i >= 0 && parent->keys[i] > key) {
                parent->keys[i+1] = parent->keys[i];
                parent->children[i+2] = parent->children[i+1];
                i--;
            }
            parent->keys[i+1] = key;
            parent->children[i+2] = right;
            if(right) right->parent = parent;
            parent->key_count++;
        } else {
            // 递归分裂父节点 (简化处理，仅支持一层分裂演示，生产环境需完整递归)
            // 这里为了代码简洁，若父节点满，暂时不分裂父节点，仅打印警告
            // 在实际项目中，你需要实现 split_internal 函数
            std::cerr << "Warning: Internal node full, split not fully implemented in this demo." << std::endl;
        }
    }

    void split_leaf(BPlusNode* leaf, int key, int value) {
        int mid = order / 2;
        BPlusNode* new_leaf = new BPlusNode(true, order);
        
        // 移动后半部分
        for (int i = mid; i < leaf->key_count; ++i) {
            new_leaf->keys[i - mid] = leaf->keys[i];
            new_leaf->values[i - mid] = leaf->values[i];
            new_leaf->key_count++;
        }
        leaf->key_count = mid;

        // 插入新值到正确位置
        if (key < new_leaf->keys[0]) {
            int i = leaf->key_count - 1;
            while (i >= 0 && leaf->keys[i] > key) {
                leaf->keys[i+1] = leaf->keys[i];
                leaf->values[i+1] = leaf->values[i];
                i--;
            }
            leaf->keys[i+1] = key;
            leaf->values[i+1] = value;
            leaf->key_count++;
        } else {
            int i = new_leaf->key_count - 1;
            while (i >= 0 && new_leaf->keys[i] > key) {
                new_leaf->keys[i+1] = new_leaf->keys[i];
                new_leaf->values[i+1] = new_leaf->values[i];
                i--;
            }
            new_leaf->keys[i+1] = key;
            new_leaf->values[i+1] = value;
            new_leaf->key_count++;
        }

        new_leaf->next_leaf = leaf->next_leaf;
        leaf->next_leaf = new_leaf;
        
        insert_into_parent(leaf, new_leaf->keys[0], new_leaf);
    }

public:
    BPlusTree(int _order = 10) : root(nullptr), order(_order) {}

    ~BPlusTree() {
        destroy_node(root);
        root = nullptr;
    }

    void insert(int key, int value) {
        if (!root) {
            root = new BPlusNode(true, order);
            root->keys[0] = key;
            root->values[0] = value;
            root->key_count = 1;
            return;
        }

        BPlusNode* leaf = find_leaf(key);
        if (!leaf) return;
        
        // 更新已存在的键
        for(int i=0; i<leaf->key_count; ++i) {
            if(leaf->keys[i] == key) {
                leaf->values[i] = value;
                return;
            }
        }

        if (leaf->key_count < order - 1) {
            int i = leaf->key_count - 1;
            while (i >= 0 && leaf->keys[i] > key) {
                leaf->keys[i+1] = leaf->keys[i];
                leaf->values[i+1] = leaf->values[i];
                i--;
            }
            leaf->keys[i+1] = key;
            leaf->values[i+1] = value;
            leaf->key_count++;
        } else {
            split_leaf(leaf, key, value);
        }
    }

    int search(int key) {
        if (!root) return -1;
        BPlusNode* leaf = find_leaf(key);
        if (!leaf) return -1;
        
        for (int i = 0; i < leaf->key_count; i++) {
            if (leaf->keys[i] == key) {
                return leaf->values[i];
            }
        }
        return -1;
    }
    
    bool remove(int key) {
        BPlusNode* leaf = find_leaf(key);
        if(!leaf) return false;
        
        for(int i=0; i<leaf->key_count; ++i) {
            if(leaf->keys[i] == key) {
                for(int j=i; j<leaf->key_count-1; ++j) {
                    leaf->keys[j] = leaf->keys[j+1];
                    leaf->values[j] = leaf->values[j+1];
                }
                leaf->key_count--;
                return true;
            }
        }
        return false;
    }
};

#endif