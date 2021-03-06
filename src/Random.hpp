#pragma once

#include <random>

class Rng
{
public:
    Rng(unsigned seed) : m_random_engine{seed} {}

    template <typename T>
    T random()
    {
        static std::uniform_real_distribution<T> distr(0, 1);
        return distr(m_random_engine);
    }

    template <typename T>
    T random(T start, T end)
    {
        std::uniform_real_distribution<T> distr(start, end);
        return distr(m_random_engine);
    }

private:
    Rng(const Rng&) = delete;
    Rng(Rng&&) = delete;
    Rng& operator=(const Rng&) = delete;
    Rng& operator=(Rng&&) = delete;

    std::mt19937 m_random_engine;
};
