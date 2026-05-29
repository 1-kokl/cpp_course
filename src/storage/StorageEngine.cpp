// src/storage/StorageEngine.cpp
#include "StorageEngine.h"
#include "Database.h"
#include "Table.h" // 需要包含 Table 以创建对象
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <unistd.h>

const std::string BASE_DIR = "./data";

bool StorageEngine::createDatabase(const std::string& dbname) {
    std::string path = BASE_DIR + "/" + dbname;
    struct stat st;
    if (stat(path.c_str(), &st) == 0) {
        return false; 
    }
    if (mkdir(path.c_str(), 0755) != 0) {
        perror("mkdir failed");
        return false;
    }
    return true;
}

bool StorageEngine::dropDatabase(const std::string& dbname) {
    std::string cmd = "rm -rf " + BASE_DIR + "/" + dbname;
    system(cmd.c_str());
    if (current_db_name == dbname) {
        delete current_db; // 【修复】释放内存
        current_db = nullptr;
        current_db_name = "";
    }
    return true;
}

Database* StorageEngine::useDatabase(const std::string& dbname) {
    std::string path = BASE_DIR + "/" + dbname;
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return nullptr; 
    }
    
    if (current_db) delete current_db; // 切换数据库时释放旧的
    current_db = new Database(); 
    current_db_name = dbname;
    return current_db;
}

Table* StorageEngine::createTable(const std::string& tablename) {
    if (!current_db) return nullptr;
    
    // 1. 创建文件 (如果不存在，createTableFile 会处理)
    createTableFile(current_db_name, tablename);
    
    // 2. 创建内存对象
    Table* t = new Table(tablename.c_str());
    
    // 3. 尝试从磁盘加载历史数据
    // 如果 .dat 文件中有数据，load_from_disk 会将它们读入内存并重建索引
    t->load_from_disk(current_db_name);
    
    // 4. 注册到数据库
    current_db->create_table(t);
    
    return t;
}

Table* StorageEngine::getTable(const std::string& tablename) {
    if (!current_db) return nullptr;
    return current_db->get_table(tablename.c_str());
}

bool StorageEngine::createTableFile(const std::string& dbname, const std::string& tablename) {
    std::string path = BASE_DIR + "/" + dbname + "/" + tablename + ".dat";
    std::ofstream file(path, std::ios::binary | std::ios::app); // 使用 append 避免覆盖
    if (!file.is_open()) return false;
    file.close();
    
    std::string idx_path = BASE_DIR + "/" + dbname + "/" + tablename + ".idx";
    std::ofstream idx_file(idx_path, std::ios::binary);
    idx_file.close();
    
    return true;
}

bool StorageEngine::insertRecordToFile(const std::string& dbname, const std::string& tablename, int row_id, const char* data, int data_len) {
    // 此方法已被 Table::insert 中的直接文件操作替代，保留以防万一
    return true;
}