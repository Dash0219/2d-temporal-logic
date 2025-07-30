#pragma once


#include <optional>
#include <queue>
#include <vector>
#include "BoundaryMap.h"
#include "Formula.h"

class BottomUpAlgorithm {
public:
    std::queue<BoundaryMap> work_set;
    std::vector<BoundaryMap> fabricated_maps;

    bool run(Formula& fmla);

private:
    std::optional<BoundaryMap> join(BoundaryMap& a, BoundaryMap& b, int direction);
    bool _join_check_continuous_edges(BoundaryMap& a, BoundaryMap& b, int direction);
    BoundaryMap _join_create_new_boundary_map(BoundaryMap& a, BoundaryMap& b, int direction);
    bool _join_defects_passed_correctly(BoundaryMap& a);
    void _join_collect_defects_along_trace(Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time);
    
    std::optional<BoundaryMap> shuffle(std::vector<BoundaryMap>& arr);

    void _add_simple_boundary_maps(Formula& fmla);
};