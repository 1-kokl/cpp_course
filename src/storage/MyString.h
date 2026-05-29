#ifndef MYSTRING_H
#define MYSTRING_H
#include <cstring>
#include <iostream> // for std::cout if needed, though not strictly required here

class MyString {
private:
    char* buf = nullptr;

public:
    MyString() = default;

    MyString(const char* s) {
        if (s) {
            buf = new char[strlen(s)+1];
            strcpy(buf, s);
        } else {
            buf = new char[1];
            buf[0] = '\0';
        }
    }

    MyString(const MyString& other) {
        if (other.buf) {
            buf = new char[strlen(other.buf)+1];
            strcpy(buf, other.buf);
        } else {
            buf = new char[1];
            buf[0] = '\0';
        }
    }

    // 【关键修复】增加赋值运算符，防止浅拷贝导致的 Double Free
    MyString& operator=(const MyString& other) {
        if (this != &other) {
            delete[] buf; // 释放旧内存
            if (other.buf) {
                buf = new char[strlen(other.buf)+1];
                strcpy(buf, other.buf);
            } else {
                buf = new char[1];
                buf[0] = '\0';
            }
        }
        return *this;
    }

    const char* c_str() const { return buf ? buf : ""; }

    ~MyString() { 
        delete[] buf; 
        buf = nullptr;
    }
};

#endif