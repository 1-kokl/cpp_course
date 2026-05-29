#ifndef DATABASE_H
#define DATABASE_H
#include "MyVector.h"
#include "Table.h"
#include "MyString.h"
#include <cstring>

class Database {
public:
    MyVector<Table*> tables;

    ~Database() {
        for (int i = 0; i < tables.size(); i++) {
            delete tables[i];
        }
    }

    void create_table(Table* t) { tables.push_back(t); }

    Table* get_table(const char* name) {
        for (int i = 0; i < tables.size(); i++) {
            if (strcmp(tables[i]->name.c_str(), name) == 0)
                return tables[i];
        }
        return nullptr;
    }
};

#endif