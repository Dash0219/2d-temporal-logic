#include "BottomUpAlgorithm.h"

bool BottomUpAlgorithm::run(Formula& fmla) {
    _add_simple_boundary_maps(fmla);
    return false;
}

// TODO: make this return a bool? like a base case even though im not recursing on anything
void BottomUpAlgorithm::_add_simple_boundary_maps(Formula& fmla) {
    for (auto& cluster : fmla.clusters) {
        BoundaryMap new_bm(cluster, fmla.content);
        fabricated_maps.push_back(new_bm);
    }
}

std::vector<BoundaryMap> BottomUpAlgorithm::join(Formula& fmla) {
    std::vector<BoundaryMap> result;
    int n = fabricated_maps.size();

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            for (int direction = 0; direction < 4; ++direction) {
                std::optional<BoundaryMap> new_bm = _join_with_direction(fabricated_maps[i], fabricated_maps[j], direction);
                if (new_bm.has_value()) result.push_back(new_bm.value());
            }
        }
    }

    return result;
}

std::optional<BoundaryMap> BottomUpAlgorithm::_join_with_direction(BoundaryMap& a, BoundaryMap& b, int direction) {
    std::optional<BoundaryMap> new_bm;
    bool join_edges_equal;
    bool side_edges_continuous_1;
    bool side_edges_continuous_2;

    if (direction == 0) { // N
        join_edges_equal = a.n == b.s;
        side_edges_continuous_1 = _join_check_continuous_edges(a, b, 1);
        side_edges_continuous_2 = _join_check_continuous_edges(a, b, 3);

        new_bm = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            _join_create_new_boundary_map(a, b, 0);
    } else if (direction == 1) { // E
        join_edges_equal = a.e == b.w;
        side_edges_continuous_1 = _join_check_continuous_edges(b, a, 0);
        side_edges_continuous_2 = _join_check_continuous_edges(b, a, 2);

        new_bm = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            _join_create_new_boundary_map(a, b, 1);
    } else if (direction == 2) { // S
        join_edges_equal = a.s == b.n;
        side_edges_continuous_1 = _join_check_continuous_edges(b, a, 1);
        side_edges_continuous_2 = _join_check_continuous_edges(b, a, 3);

        new_bm = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt : 
            _join_create_new_boundary_map(b, a, 0);
    } else if (direction == 3) { // W
        join_edges_equal = a.w == b.e;
        side_edges_continuous_1 = _join_check_continuous_edges(a, b, 0);
        side_edges_continuous_2 = _join_check_continuous_edges(a, b, 2);

        new_bm = (!join_edges_equal || !side_edges_continuous_1 || !side_edges_continuous_2) ?
            std::nullopt :
            _join_create_new_boundary_map(b, a, 1);
    } else { // invalid
        new_bm = std::nullopt;
    }

    // return (new_bm.has_value() && !new_bm.value().has_internal_defects()) ?
    return (new_bm.has_value() && _join_defects_passed_correctly(new_bm.value())) ?
        new_bm : std::nullopt;
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
// returns std::nullopt if the join fails because joining either two edge traces fails
std::optional<BoundaryMap> BottomUpAlgorithm::_join_create_new_boundary_map(BoundaryMap& a, BoundaryMap& b, int direction) {
    BoundaryMap new_bm;

    if (direction == 0) { // N
        new_bm.l = b.l;
        new_bm.r = a.r;
        new_bm.b = a.b;
        new_bm.t = b.t;
        
        new_bm.n = b.n;
        if (a.e.has_value() && a.t.has_value() && b.e.has_value()) {
            Trace new_e = a.e.value(); 
            if (new_e.push_back(a.t.value()) && new_e.join(b.e.value())) {
                new_bm.e = new_e;
            } else {
                return std::nullopt;
            }
        } else {
            new_bm.e = std::nullopt;
        }
        new_bm.s = a.s;
        if (a.w.has_value() && a.l.has_value() && b.w.has_value()) {
            Trace new_w = a.w.value(); 
            if (new_w.push_back(a.l.value()) && new_w.join(b.w.value())) {
                new_bm.w = new_w;
            } else {
                return std::nullopt;
            }
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
            Trace new_n = a.n.value(); 
            if (new_n.push_back(a.t.value()) && new_n.join(b.n.value())) {
                new_bm.n = new_n;
            } else {
                return std::nullopt;
            }
        } else {
            new_bm.n = std::nullopt;
        }
        new_bm.e = b.e;
        if (a.s.has_value() && a.r.has_value() && b.s.has_value()) {
            Trace new_s = a.w.value(); 
            if (new_s.push_back(a.r.value()) && new_s.join(b.s.value())) {
                new_bm.s = new_s;
            } else {
                return std::nullopt;
            }
        } else {
            new_bm.s = std::nullopt;
        }
        new_bm.w = a.w;

        new_bm.plus = b.plus;
        new_bm.minus = a.minus;
    } 

    return std::make_optional(new_bm);
}

bool BottomUpAlgorithm::_join_defects_passed_correctly(BoundaryMap& a) {
    std::unordered_set<std::string> n_future_defects;
    std::unordered_set<std::string> e_future_defects;
    std::unordered_set<std::string> s_past_defects;
    std::unordered_set<std::string> w_past_defects;
    
    // TODO: after collecting defects along one edge, check if they're in the top corner, what if theres no top corner?
    // TODO: shouldn't _join_collect_defects_along_trace() be in the Trace class lol?
    // maybe not if you say Trace is a data class so it shouldn't do weird shit like this which is fair
    if (a.n.has_value()) _join_collect_defects_along_trace(a.n.value(), n_future_defects, false);
    if (a.e.has_value()) _join_collect_defects_along_trace(a.e.value(), e_future_defects, false);
    if (a.plus.has_value()) {
        for (auto& fmla : n_future_defects) {
            if (!a.plus.value().formulas.contains(fmla) && !a.plus.value().formulas.contains(fmla.substr(1)))
                return false;
        }
        for (auto& fmla : e_future_defects) {
            if (!a.plus.value().formulas.contains(fmla) && !a.plus.value().formulas.contains(fmla.substr(1)))
                return false;
        }
    }

    if (a.s.has_value()) _join_collect_defects_along_trace(a.s.value(), s_past_defects, true);
    if (a.w.has_value()) _join_collect_defects_along_trace(a.w.value(), w_past_defects, true);
    if (a.minus.has_value()) {
        for (auto& fmla : s_past_defects) {
            if (!a.minus.value().formulas.contains(fmla) && !a.minus.value().formulas.contains(fmla.substr(1)))
                return false;
        }
        for (auto& fmla : w_past_defects) {
            if (!a.minus.value().formulas.contains(fmla) && !a.minus.value().formulas.contains(fmla.substr(1)))
                return false;
        }
    }

    return true;
}

void BottomUpAlgorithm::_join_collect_defects_along_trace(Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time) {
    if (reverse_time) { // going from future -> past, collecting past defects
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
    } else { // going from past -> future, collecting future defects
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

std::vector<BoundaryMap> BottomUpAlgorithm::shuffle(Formula& fmla) {
    std::vector<BoundaryMap> result;
    int n = fmla.clusters.size();
    int m = fabricated_maps.size();

    // guess a pair of clusters to be the plus and minus
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            // exclude simple bm, would've already been added to the list
            // exclude clusters pairs in the wrong order
            if (i == j || fmla.clusters[j] <= fmla.clusters[i]) continue; 
            
            // guess an array of boundary maps in O(exp(m)), yikes...
            std::vector<BoundaryMap> arr;
            _shuffle_guess_array(result, fmla.clusters[i], fmla.clusters[j], arr, 0, m);
        }
    }

    return result;
}

void BottomUpAlgorithm::_shuffle_guess_array(std::vector<BoundaryMap>& result, Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr, int i, int max_arr_size) {
    if (i == max_arr_size) return;

    std::optional<BoundaryMap> new_bm = _shuffle_create_new_boundary_map(plus, minus, arr);
    if (new_bm.has_value()) result.push_back(new_bm.value());

    arr.push_back(fabricated_maps[i]);
    _shuffle_guess_array(result, plus, minus, arr, i + 1, max_arr_size);
    arr.pop_back();
    _shuffle_guess_array(result, plus, minus, arr, i + 1, max_arr_size);
}

// returns std::nullopt if shuffle fails 
std::optional<BoundaryMap> BottomUpAlgorithm::_shuffle_create_new_boundary_map(Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr) {
    if (arr.empty()) return std::nullopt;

    BoundaryMap new_bm;
    bool contains_formula = false;
    // TODO: decide later whether to only add BMs with b and t to the arr, or check here
    // i'll check here first, but checking in whatever function calls this one should be more optimized
    for (auto& bm : arr) {
        if (!bm.b.has_value() || !bm.t.has_value()) return std::nullopt;
        if (bm.b.value() <= minus || plus <= bm.t.value()) return std::nullopt;
        contains_formula |= bm.contains_formula;
    }

    new_bm.plus = plus;
    new_bm.minus = minus;
    new_bm.contains_formula = contains_formula;

    return std::make_optional(new_bm);
}
