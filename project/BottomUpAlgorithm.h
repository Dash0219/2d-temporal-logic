#pragma once


#include "Formula.h"
#include "BoundaryMap.h"

class BottomUpAlgorithm {
public:
    using Element = std::variant<MaximalConsistentSet*, Cluster*>;

    // std::vector<BoundaryMap> one_point_maps;
    // std::vector<BoundaryMap> fabricated_maps;
    Formula& formula;
    std::unordered_set<BoundaryMap> fabricated_maps;
    
    BottomUpAlgorithm(Formula& formula);

    bool run(bool print_debug);

private:
    void _add_simple_boundary_maps();
    void _add_one_point_boundary_maps();
    std::optional<BoundaryMap> check_for_solution(std::unordered_set<BoundaryMap>& maps);
    
    std::unordered_set<BoundaryMap> join();
    std::optional<BoundaryMap> _join_with_direction(const BoundaryMap& a, const BoundaryMap& b, int direction);
    bool _join_check_continuous_edges(const BoundaryMap& a, const BoundaryMap& b, int direction);
    std::optional<BoundaryMap> _join_create_new_boundary_map(const BoundaryMap& a, const BoundaryMap& b, int direction);
    bool _join_defects_passed_correctly(const BoundaryMap& a);
    void _join_collect_defects_along_trace(const Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time);

    std::unordered_set<BoundaryMap> shuffle();
    std::optional<BoundaryMap> _shuffle_find_shuffle(Cluster* plus, Cluster* minus);
    BoundaryMap _shuffle_create_new_boundary_map(Cluster* plus, Cluster* minus, bool contains_formula);

    std::unordered_set<BoundaryMap> close();
    void _close_add_edges(const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int direction);
    void _close_add_corners(const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int corner);
};