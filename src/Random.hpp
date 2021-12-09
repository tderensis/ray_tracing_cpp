#pragma once

#include <random>

class Rng
{
public:

    static Rng& instance()
    {
        static Rng INSTANCE{(unsigned)time(0)};
        return INSTANCE;
    }

    template <typename T, T start=(T)0, T end=(T)1>
    T random()
    {
        static std::uniform_real_distribution<T> distr(start, end);
        return distr(m_random_engine);
    }

    template <typename T>
    T random(T start, T end)
    {
        std::uniform_real_distribution<T> distr(start, end);
        return distr(m_random_engine);
    }
private:

    Rng(unsigned seed) : m_random_engine{seed} {}
    ~Rng() = default;
    Rng(const Rng&) = delete;
    Rng(Rng&&) = delete;
    Rng& operator=(const Rng&) = delete;
    Rng& operator=(Rng&&) = delete;

    std::mt19937 m_random_engine;
};
