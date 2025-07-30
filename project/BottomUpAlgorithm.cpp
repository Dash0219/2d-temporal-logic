#include "BottomUpAlgorithm.h"

bool BottomUpAlgorithm::run(Formula& fmla) {
    _add_simple_boundary_maps(fmla);
    return false;
}

void BottomUpAlgorithm::_add_simple_boundary_maps(Formula& fmla) {
    for (auto& cluster : fmla.clusters) {
        BoundaryMap new_bm(cluster);
        fabricated_maps.push_back(new_bm);
    }
}

std::optional<BoundaryMap> BottomUpAlgorithm::join(BoundaryMap& a, BoundaryMap& b, int direction) {
    std::optional<BoundaryMap> result;
    bool join_edges_equal;
    bool side_edges_continuous_1;
    bool side_edges_continuous_2;

    if (direction == 0) { // N
        join_edges_equal = a.n == b.s;
        side_edges_continuous_1 = _join_check_continuous_edges(a, b, 1);
        side_edges_continuous_2 = _join_check_continuous_edges(a, b, 3);

        result = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            std::make_optional(_join_create_new_boundary_map(a, b, 0));
    } else if (direction == 1) { // E
        join_edges_equal = a.e == b.w;
        side_edges_continuous_1 = _join_check_continuous_edges(b, a, 0);
        side_edges_continuous_2 = _join_check_continuous_edges(b, a, 2);

        result = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            std::make_optional(_join_create_new_boundary_map(a, b, 1));
    } else if (direction == 2) { // S
        join_edges_equal = a.s == b.n;
        side_edges_continuous_1 = _join_check_continuous_edges(b, a, 1);
        side_edges_continuous_2 = _join_check_continuous_edges(b, a, 3);

        result = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt : 
            std::make_optional(_join_create_new_boundary_map(b, a, 0));
    } else if (direction == 3) { // W
        join_edges_equal = a.w == b.e;
        side_edges_continuous_1 = _join_check_continuous_edges(a, b, 0);
        side_edges_continuous_2 = _join_check_continuous_edges(a, b, 2);

        result = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            std::make_optional(_join_create_new_boundary_map(b, a, 1));
    } else { // invalid
        result = std::nullopt;
    }

    // return (result.has_value() && !result.value().has_internal_defects()) ?
    return (result.has_value() && _join_defects_passed_correctly(result.value())) ?
        result : std::nullopt;
}

// direction is the edge we're checking
// either both edges and both corners that join at the middle exist or they all don't
// checks go forward in time, ordering of args matters
bool BottomUpAlgorithm::_join_check_continuous_edges(BoundaryMap& a, BoundaryMap& b, int direction) {
    if (direction == 0) { // N
        if (a.n.has_value() && b.n.has_value() && a.t.has_value() && b.l.has_value()) {
            return a.t == b.l;
        } else if (!a.n.has_value() && !b.n.has_value() && !a.t.has_value() && !b.l.has_value()) {
            return true;
        } else {
            return false;
        }
    } else if (direction == 1) { // E
        if (a.e.has_value() && b.e.has_value() && a.t.has_value() && b.r.has_value()) {
            return a.t == b.r;
        } else if (!a.e.has_value() && !b.e.has_value() && !a.t.has_value() && !b.r.has_value()) {
            return true;
        } else {
            return false;
        }
    } else if (direction == 2) { // S
        if (a.s.has_value() && b.s.has_value() && a.r.has_value() && b.b.has_value()) {
            return a.r == b.b;
        } else if (!a.s.has_value() && !b.s.has_value() && !a.r.has_value() && !b.b.has_value()) {
            return true;
        } else {
            return false;
        }
    } else if (direction == 3) { // W
        if (a.w.has_value() && b.w.has_value() && a.l.has_value() && b.b.has_value()) {
            return a.l == b.b;
        } else if (!a.w.has_value() && !b.w.has_value() && !a.l.has_value() && !b.b.has_value()) {
            return true;
        } else {
            return false;
        }
    }

    return false;
}

// TODO: clarify this
// does the NESW traces include the corners, the new algo pdf says no

// only N and E joins, swap args for S and W joins
BoundaryMap BottomUpAlgorithm::_join_create_new_boundary_map(BoundaryMap& a, BoundaryMap& b, int direction) {
    BoundaryMap new_bm;

    if (direction == 0) { // N
        new_bm.l = b.l;
        new_bm.r = a.r;
        new_bm.b = a.b;
        new_bm.t = b.t;
        
        new_bm.n = b.n;
        if (a.e.has_value() && a.t.has_value() && b.e.has_value()) {
            Trace new_e = a.e.value(); new_e.push_back(a.t.value()); new_e.join(b.e.value());
            new_bm.e = new_e;
        } else {
            new_bm.e = std::nullopt;
        }
        new_bm.s = a.s;
        if (a.w.has_value() && a.l.has_value() && b.w.has_value()) {
            Trace new_w = a.w.value(); new_w.push_back(a.l.value()); new_w.join(b.w.value());
            new_bm.w = new_w;
        } else {
            new_bm.w = std::nullopt;
        }

        new_bm.plus = b.plus;
        new_bm.minus = a.minus;
    } else if (direction == 1) { // E
        new_bm.l = a.l;
        new_bm.r = b.r;
        new_bm.b = a.b;
        new_bm.t = b.t;
        
        if (a.n.has_value() && a.t.has_value() && b.n.has_value()) {
            Trace new_n = a.n.value(); new_n.push_back(a.t.value()); new_n.join(b.n.value());
            new_bm.n = new_n;
        } else {
            new_bm.n = std::nullopt;
        }
        new_bm.e = b.e;
        if (a.s.has_value() && a.r.has_value() && b.s.has_value()) {
            Trace new_s = a.w.value(); new_s.push_back(a.r.value()); new_s.join(b.s.value());
            new_bm.s = new_s;
        } else {
            new_bm.s = std::nullopt;
        }
        new_bm.w = a.w;

        new_bm.plus = b.plus;
        new_bm.minus = a.minus;
    } 

    return new_bm;
}

bool BottomUpAlgorithm::_join_defects_passed_correctly(BoundaryMap& a) {
    std::unordered_set<std::string> past_defects;
    std::unordered_set<std::string> future_defects;

    if (a.n.has_value()) _join_collect_defects_along_trace(a.n.value(), future_defects, false);
    if (a.w.has_value()) _join_collect_defects_along_trace(a.n.value(), future_defects, false);


    return true;
}

void BottomUpAlgorithm::_join_collect_defects_along_trace(Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time) {
    if (reverse_time) {
        for (auto it = trace.sequence.rbegin(); it != trace.sequence.rend(); ++it) {
            auto& elem = *it;
            if (std::holds_alternative<Cluster>(elem)) {
                for (auto& fmla : std::get<Cluster>(elem).formulas) {
                    std::string past_fmla = std::string('P', 1) + fmla;
                    if (defects.contains(past_fmla)) defects.erase(past_fmla);
                }
            } else {
                for (auto& fmla : std::get<MaximalConsistentSet>(elem).formulas) {
                    std::string past_fmla = std::string('P', 1) + fmla;
                    if (fmla[0] == 'P') defects.insert(fmla.substr(1));
                    if (defects.contains(past_fmla)) defects.erase(past_fmla);
                }
            }
        }
    } else {
        for (auto& elem : trace.sequence) {
            if (std::holds_alternative<Cluster>(elem)) {
                for (auto& fmla : std::get<Cluster>(elem).formulas) {
                    std::string future_fmla = std::string('F', 1) + fmla;
                    if (defects.contains(future_fmla)) defects.erase(future_fmla);
                }
            } else {
                for (auto& fmla : std::get<MaximalConsistentSet>(elem).formulas) {
                    std::string future_fmla = std::string('F', 1) + fmla;
                    if (fmla[0] == 'F') defects.insert(fmla.substr(1));
                    if (defects.contains(future_fmla)) defects.erase(future_fmla);
                }
            }
        }
    }
}

