#pragma once
template <typename T>
class Singleton
{
public:
    static T& getInstance()
    {
        static T instance;
        return instance;
    }

protected:
    Singleton() {}
    ~Singleton() {}
public:

    Singleton(const Singleton&) = delete;
    Singleton& operator= (const Singleton&) = delete;

    Singleton& operator= (Singleton&&) = delete;
    Singleton(Singleton&&) = delete;
};

