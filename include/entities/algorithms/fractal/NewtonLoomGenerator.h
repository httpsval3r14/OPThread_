#pragma once
#include "entities/graph.h"
#include "NewtonField.h"
#include <vector>

namespace entities::algorithms::fractal {

    class NewtonLoomGenerator {
    public:
        struct ConnectOptions {
            bool useDiagonal = true;
            bool useGradientEdges = true;
            int knn = 4;
            float maxDistance = 80.0f;
        };

        explicit NewtonLoomGenerator(const ConnectOptions& opts = ConnectOptions());

        void generate(Graph& graph, const NewtonField& field) const;

    private:
        ConnectOptions opts_;

        void connectGrid(Graph& graph, const std::vector<NewtonField::Cell>& cells,
            const std::vector<std::vector<uint64_t>>& ids, int res) const;
        void connectDiagonals(Graph& graph, const std::vector<NewtonField::Cell>& cells,
            const std::vector<std::vector<uint64_t>>& ids, int res) const;
        void connectKNN(Graph& graph, const std::vector<NewtonField::Cell>& cells,
            const std::vector<uint64_t>& flatIds, int k) const;
    };

} // namespace entities::algorithms::fractal