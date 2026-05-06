#include "entities/node.h"
#include <cmath>
#include <cstdint>

namespace {
    uint32_t zigzagEncode(int32_t n) {
        return (static_cast<uint32_t>(n) << 1) ^ static_cast<uint32_t>(n >> 31);
    }
}

namespace entities {
    Node::Node(float raw_x, float raw_y) {
        int32_t qx = static_cast<int32_t>(std::round(raw_x));
        int32_t qy = static_cast<int32_t>(std::round(raw_y));

        this->x = static_cast<float>(qx);
        this->y = static_cast<float>(qy);

        uint32_t zx = zigzagEncode(qx);
        uint32_t zy = zigzagEncode(qy);

        this->id = (static_cast<uint64_t>(zx) << 32) | zy;
    }

    void Node::removeEdge(uint64_t targetDestinationID) {
        neighbors.erase(targetDestinationID);
        weights.erase(targetDestinationID);
    }
}