#pragma once
#include <cstdint>
#include <random>
#include <string>
#include <iostream>
#include <chrono>

namespace entities {

    class SeedManager {
    public:
        static SeedManager& instance() {
            static SeedManager s;
            return s;
        }

        void initRandom() {
            seed_ = static_cast<uint32_t>(
                std::chrono::high_resolution_clock::now()
                .time_since_epoch().count() & 0xFFFFFFFF
                );
            rng_.seed(seed_);
        }

        void initFixed(uint32_t s) {
            seed_ = s;
            rng_.seed(seed_);
        }

        std::mt19937& rng() { return rng_; }
        uint32_t seed() const { return seed_; }

        void printStats(const std::string& modeName,
            std::size_t nodeCount,
            std::size_t edgeCount) const
        {
            std::cout << "Seed: " << seed_ << "\n";
            std::cout << "Modo: " << modeName << "\n";
            std::cout << "Nodos: " << nodeCount << "\n";
            std::cout << "Aristas: " << edgeCount << "\n\n";
        }

    private:
        SeedManager() = default;
        uint32_t     seed_{ 0 };
        std::mt19937 rng_;
    };

}