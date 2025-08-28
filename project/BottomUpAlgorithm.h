#pragma once


#include "BoundaryMap.h"
#include "Formula.h"

class BottomUpAlgorithm {
public:
    using Element = std::variant<MaximalConsistentSet*, Cluster*>;
    // std::vector<BoundaryMap> one_point_maps;
    // std::vector<BoundaryMap> fabricated_maps;
    std::unordered_set<BoundaryMap> fabricated_maps;

    bool run(Formula& fmla, bool print_debug);

private:
    void _add_simple_boundary_maps(Formula& fmla);
    void _add_one_point_boundary_maps(Formula& fmla);
    std::optional<BoundaryMap> check_for_solution(std::unordered_set<BoundaryMap>& maps);
    
    std::unordered_set<BoundaryMap> join(Formula& fmla);
    std::optional<BoundaryMap> _join_with_direction(Formula& fmla, const BoundaryMap& a, const BoundaryMap& b, int direction);
    bool _join_check_continuous_edges(const BoundaryMap& a, const BoundaryMap& b, int direction);
    std::optional<BoundaryMap> _join_create_new_boundary_map(Formula& fmla, const BoundaryMap& a, const BoundaryMap& b, int direction);
    bool _join_defects_passed_correctly(const BoundaryMap& a);
    void _join_collect_defects_along_trace(const Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time);
    
    // std::vector<BoundaryMap> shuffle(Formula& fmla);
    // void _shuffle_guess_array(std::vector<BoundaryMap>& result, Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr, int i, int max_arr_size);
    // std::optional<BoundaryMap> _shuffle_create_new_boundary_map(Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr);

    std::unordered_set<BoundaryMap> shuffle(Formula& fmla);
    std::optional<BoundaryMap> _shuffle_find_shuffle(Cluster* plus, Cluster* minus);
    BoundaryMap _shuffle_create_new_boundary_map(Cluster* plus, Cluster* minus, bool contains_formula);

    std::unordered_set<BoundaryMap> close(Formula& fmla);
    void _close_add_edges(Formula& fmla, const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int direction);
    void _close_add_corners(Formula& fmla, const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int corner);
};