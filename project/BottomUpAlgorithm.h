#pragma once


#include <utility>
#include "Formula.h"
#include "BoundaryMap.h"


struct BoundaryMapPairHash {
    size_t operator()(const std::pair<BoundaryMap, BoundaryMap>& p) const noexcept {
        size_t h1 = std::hash<BoundaryMap>{}(p.first);
        size_t h2 = std::hash<BoundaryMap>{}(p.second);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
    }
};


struct BoundaryMapPairEqual {
    bool operator()(const std::pair<BoundaryMap, BoundaryMap>& lhs,
                    const std::pair<BoundaryMap, BoundaryMap>& rhs) const noexcept {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};


class BottomUpAlgorithm {
public:
    using Element = std::variant<MaximalConsistentSet*, Cluster*>;

    // std::vector<BoundaryMap> one_point_maps;
    // std::vector<BoundaryMap> fabricated_maps;
    Formula& formula;
    std::unordered_set<BoundaryMap> fabricated_maps;
    // std::unordered_set<std::pair<BoundaryMap, BoundaryMap>, BoundaryMapPairHash, BoundaryMapPairEqual> attempted_join;
    // std::unordered_set<std::pair<long long, long long>> attempted_join;
    // std::unordered_set<std::pair<Cluster*, Cluster*>> attempted_shuffle;

    
    BottomUpAlgorithm(Formula& formula);

    bool run(bool print_debug);

private:
    void _add_simple_boundary_maps();
    void _add_one_point_boundary_maps();
    std::optional<BoundaryMap> check_for_solution(std::unordered_set<BoundaryMap>& maps);
    
    std::unordered_set<BoundaryMap> join();
    std::optional<BoundaryMap> _join_with_direction(const BoundaryMap& a, const BoundaryMap& b, int direction);
    bool _join_check_continuous_edges(const BoundaryMap& a, const BoundaryMap& b, int edge);
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
