#pragma once

template<class T>
struct Singleton {
    static T& getInstance() {
        static T instance;
        return instance;
    }
protected:
    Singleton() {}
};