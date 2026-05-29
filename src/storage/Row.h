#ifndef ROW_H
#define ROW_H
#include "MyVector.h"
#include "MyString.h"

class Row {
public:
    MyVector<int> int_vals;
    MyVector<MyString> str_vals;

    Row() {}
    
    // 拷贝构造函数，用于 insert 时复制
    Row(const Row& other) {
        for(int i=0; i<other.int_vals.size(); ++i) {
            int_vals.push_back(other.int_vals[i]);
        }
        for(int i=0; i<other.str_vals.size(); ++i) {
            str_vals.push_back(other.str_vals[i]);
        }
    }

    void add_int(int v) { int_vals.push_back(v); }
    void add_str(const char* s) { str_vals.push_back(MyString(s)); }
};

#endif