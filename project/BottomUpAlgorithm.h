#pragma once


#include <optional>
#include <queue>
#include <vector>
#include "BoundaryMap.h"
#include "Formula.h"

class BottomUpAlgorithm {
public:
    std::queue<BoundaryMap> work_set;
    // std::vector<BoundaryMap> one_point_maps;
    std::vector<BoundaryMap> fabricated_maps;

    bool run(Formula& fmla);

private:
    void _add_simple_boundary_maps(Formula& fmla);
    void _add_one_point_boundary_maps(Formula& fmla);
    
    std::vector<BoundaryMap> join(Formula& fmla);
    std::optional<BoundaryMap> _join_with_direction(BoundaryMap& a, BoundaryMap& b, int direction);
    bool _join_check_continuous_edges(BoundaryMap& a, BoundaryMap& b, int direction);
    std::optional<BoundaryMap> _join_create_new_boundary_map(BoundaryMap& a, BoundaryMap& b, int direction);
    bool _join_defects_passed_correctly(BoundaryMap& a);
    void _join_collect_defects_along_trace(Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time);
    
    // std::vector<BoundaryMap> shuffle(Formula& fmla);
    // void _shuffle_guess_array(std::vector<BoundaryMap>& result, Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr, int i, int max_arr_size);
    // std::optional<BoundaryMap> _shuffle_create_new_boundary_map(Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr);

    std::vector<BoundaryMap> shuffle(Formula& fmla);
    void _shuffle_iterate_array(std::vector<BoundaryMap>& result, Cluster& plus, Cluster& minus);
    BoundaryMap _shuffle_create_new_boundary_map(Cluster& plus, Cluster& minus, bool contains_formula);

    std::vector<BoundaryMap> close(Formula& fmla);
    
};