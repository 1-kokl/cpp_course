// src/storage/Table.h
#ifndef TABLE_H
#define TABLE_H

#include "MyVector.h"
#include "Column.h"
#include "Row.h"
#include "BPlusTree.h"
#include <string>

class Table {
public:
    MyString name;
    MyVector<Column> columns;
    MyVector<Row*> rows; // 内存缓存
    BPlusTree* primary_index; // B+树索引
    int next_row_id; // 持久化的 Row ID 计数器

    Table(const char* n);
    ~Table();

    void add_column(const Column& col);
    void insert(const Row& r);
    bool remove_by_primary_key(int key);
    bool update_by_primary_key(int key, const Row& new_r);
    Row* select_by_primary_key(int key);
    MyVector<Row*> scan_all(); // 全表扫描
    
    // 持久化辅助
    void save_to_disk(const std::string& dbname);
    void load_from_disk(const std::string& dbname);
};

#endif