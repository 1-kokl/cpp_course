// test/test_storage.cpp
#include <iostream>
#include "../src/storage/StorageEngine.h"
#include "../src/storage/Table.h"
#include "../src/storage/Column.h"
#include "../src/storage/Row.h"

int main() {
    StorageEngine& engine = StorageEngine::getInstance();
    
    // 【演示用】每次测试前清空旧数据，确保环境干净
    engine.dropDatabase("test_db"); 
    
    if (!engine.createDatabase("test_db")) {
        std::cout << "Database created." << std::endl;
    }
    
    Database* db = engine.useDatabase("test_db");
    if (!db) {
        std::cerr << "Failed to use database." << std::endl;
        return 1;
    }

    // 1. 创建表 (内部会自动调用 load_from_disk)
    Table* t = engine.createTable("user");
    if (!t) {
        std::cerr << "Failed to create table." << std::endl;
        return 1;
    }

    t->add_column(Column("id", 0, true)); 
    t->add_column(Column("name", 1, false)); 
    
    // 2. 检查当前表中有多少数据
    MyVector<Row*> existing_rows = t->scan_all();
    std::cout << "Current rows in table: " << existing_rows.size() << std::endl;

    // 3. 如果没有数据，则插入一些测试数据
    if (existing_rows.size() == 0) {
        std::cout << "--- Inserting New Data ---" << std::endl;
        Row r1;
        r1.add_int(1001);
        r1.add_str("zhangsan");
        t->insert(r1);

        Row r2;
        r2.add_int(1002);
        r2.add_str("lisi");
        t->insert(r2);
        std::cout << "Data inserted. Please run the program again to see persistence." << std::endl;
    } else {
        std::cout << "--- Data Loaded from Disk ---" << std::endl;
        for(int i=0; i<existing_rows.size(); ++i) {
            std::cout << "  Loaded: ID=" << existing_rows[i]->int_vals[0] 
                      << ", Name=" << existing_rows[i]->str_vals[0].c_str() << std::endl;
        }
    }

    // 4. 验证查询功能
    Row* found = t->select_by_primary_key(1001);
    if (found) {
        std::cout << "Select Test Passed: Found ID=1001" << std::endl;
    }

    std::cout << "Test Completed." << std::endl;
    return 0;
}