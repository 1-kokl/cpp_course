#ifndef MYVECTOR_H
#define MYVECTOR_H

template <typename T>
class MyVector {
private:
    T* arr = nullptr;
    int len = 0;
    int cap = 0;

    void resize() {
        if (cap == 0) cap = 4;
        else cap *= 2;
        T* new_arr = new T[cap];
        for (int i = 0; i < len; i++) new_arr[i] = arr[i];
        delete[] arr;
        arr = new_arr;
    }

public:
    MyVector() = default;

    void push_back(const T& val) {
        if (len >= cap) resize();
        arr[len++] = val;
    }

    T& operator[](int idx) { return arr[idx]; }
    const T& operator[](int idx) const { return arr[idx]; }

    int size() const { return len; }
    void clear() { len = 0; }

    ~MyVector() { delete[] arr; }
};

#endif
