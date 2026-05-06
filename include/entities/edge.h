#pragma once
#include <cstdint>
#include <cmath>

namespace entities {
	class Edge {
	private:
		uint64_t destination;
		uint32_t weight;
		int angle;

	public:
		Edge(uint64_t dest, float raw_weight, int ang = 0) :
			destination(dest),
			weight(static_cast<uint32_t>(std::round(raw_weight * 100.0f))),
			angle(ang) {
		}

		uint64_t getDestination() const { return destination; }
		uint32_t getWeight() const { return weight; }
		int getAngle() const { return angle; }

		bool operator < (const Edge& other) const {
			return this->weight < other.weight;
		}

		bool operator == (const Edge& other) const {
			return this->destination == other.destination &&
				this->weight == other.weight;
		}
	};
}