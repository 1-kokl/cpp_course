// src/storage/StorageEngine.h
#ifndef STORAGE_ENGINE_H
#define STORAGE_ENGINE_H

#include <string>

class Database; 
class Table;

class StorageEngine {
public:
    static StorageEngine& getInstance() {
        static StorageEngine instance;
        return instance;
    }

    bool createDatabase(const std::string& dbname);
    bool dropDatabase(const std::string& dbname);
    Database* useDatabase(const std::string& dbname);
    
    // 【新增】创建表并注册到当前数据库
    Table* createTable(const std::string& tablename);
    // 【新增】获取表
    Table* getTable(const std::string& tablename);
    
    bool createTableFile(const std::string& dbname, const std::string& tablename);
    bool insertRecordToFile(const std::string& dbname, const std::string& tablename, int row_id, const char* data, int data_len);
    
    Database* getCurrentDB() { return current_db; }
    std::string getCurrentDBName() { return current_db_name; }

private:
    StorageEngine() : current_db(nullptr) {}
    Database* current_db;
    std::string current_db_name;
    std::string base_dir; 
};

#endif