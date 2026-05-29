#ifndef COLUMN_H
#define COLUMN_H
#include "MyString.h"

class Column {
public:
    MyString name;
    int type;
    bool is_primary;

    Column() : type(0), is_primary(false) {}  // 修复在这里！
    Column(const char* n, int t, bool pri) : name(n), type(t), is_primary(pri) {}
};

#endif
