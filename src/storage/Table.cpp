// src/storage/Table.cpp
#include "Table.h"
#include "StorageEngine.h"
#include <fstream>
#include <iostream>

Table::Table(const char* n) : name(n), primary_index(nullptr), next_row_id(0) {
}

Table::~Table() {
    for (int i = 0; i < rows.size(); i++) {
        delete rows[i];
    }
    if (primary_index) delete primary_index;
}

void Table::add_column(const Column& col) {
    columns.push_back(col);
    if (col.is_primary && !primary_index) {
        primary_index = new BPlusTree(10);
    }
}

void Table::insert(const Row& r) {
    if (!primary_index) {
        std::cerr << "Error: No primary key index." << std::endl;
        return;
    }

    int pk = r.int_vals[0]; 
    
    // 检查主键冲突
    if (primary_index->search(pk) != -1) {
        std::cerr << "Error: Duplicate Primary Key " << pk << std::endl;
        return;
    }

    int row_id = next_row_id++;
    
    // 1. 更新内存
    Row* new_row = new Row(r);
    rows.push_back(new_row);
    
    // 2. 更新 B+树
    primary_index->insert(pk, row_id);
    
    // 3. 持久化 (追加模式)
    std::string db_name = StorageEngine::getInstance().getCurrentDBName();
    if (!db_name.empty()) {
        std::string path = "./data/" + db_name + "/" + name.c_str() + ".dat";
        std::ofstream file(path, std::ios::binary | std::ios::app);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&row_id), sizeof(int));
            file.write(reinterpret_cast<const char*>(&pk), sizeof(int));
            int str_len = strlen(r.str_vals[0].c_str());
            file.write(reinterpret_cast<const char*>(&str_len), sizeof(int));
            file.write(r.str_vals[0].c_str(), str_len);
            file.close();
        }
    }
}

bool Table::remove_by_primary_key(int key) {
    if (!primary_index) return false;
    
    // 1. 从索引删除
    if (!primary_index->remove(key)) return false;

    // 2. 从内存删除 (使用 MyVector 的安全方式：这里简单遍历查找并标记，实际应优化)
    for (int i = 0; i < rows.size(); i++) {
        if (!rows[i]->int_vals.empty() && rows[i]->int_vals[0] == key) {
            delete rows[i];
            // 简单移除：将最后一个元素移过来，然后减少长度
            // 注意：这要求 MyVector 暴露内部操作或提供 remove_at
            // 由于 MyVector 没提供 remove_at，我们只能手动操作 arr 和 len (不推荐但可行)
            rows.arr[i] = rows.arr[rows.len - 1];
            rows.len--;
            return true;
        }
    }
    return false;
}

bool Table::update_by_primary_key(int key, const Row& new_r) {
    if (remove_by_primary_key(key)) {
        insert(new_r);
        return true;
    }
    return false;
}

Row* Table::select_by_primary_key(int key) {
    if (!primary_index) return nullptr;
    
    int row_id = primary_index->search(key);
    if (row_id == -1) return nullptr;
    
    for (int i = 0; i < rows.size(); i++) {
        if (!rows[i]->int_vals.empty() && rows[i]->int_vals[0] == key) {
            return rows[i];
        }
    }
    return nullptr;
}

MyVector<Row*> Table::scan_all() {
    MyVector<Row*> result;
    for (int i = 0; i < rows.size(); i++) {
        result.push_back(rows[i]);
    }
    return result;
}

void Table::load_from_disk(const std::string& dbname) {
    std::string path = "./data/" + dbname + "/" + name.c_str() + ".dat";
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return;

    // 清空当前内存状态，重新加载
    for (int i = 0; i < rows.size(); i++) delete rows[i];
    rows.len = 0;
    if(primary_index) delete primary_index;
    primary_index = new BPlusTree(10);
    next_row_id = 0;

    while (file.good()) {
        int row_id, pk, str_len;
        file.read(reinterpret_cast<char*>(&row_id), sizeof(int));
        if (!file.good()) break;
        
        file.read(reinterpret_cast<char*>(&pk), sizeof(int));
        file.read(reinterpret_cast<char*>(&str_len), sizeof(int));
        
        if(str_len > 1024 || str_len < 0) break; // 防止损坏数据导致崩溃

        char* buf = new char[str_len + 1];
        file.read(buf, str_len);
        buf[str_len] = '\0';

        Row r;
        r.add_int(pk);
        r.add_str(buf);
        delete[] buf;

        // 直接插入内存和索引，但不重复写文件
        Row* new_row = new Row(r);
        rows.push_back(new_row);
        primary_index->insert(pk, row_id);
        
        if (row_id >= next_row_id) next_row_id = row_id + 1;
    }
    file.close();
}

void Table::save_to_disk(const std::string& dbname) {
    // 全量保存逻辑（可选）
}