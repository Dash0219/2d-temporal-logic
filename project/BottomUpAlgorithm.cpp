#include "BottomUpAlgorithm.h"


bool BottomUpAlgorithm::run(Formula& fmla, bool print_debug) {
    _add_simple_boundary_maps(fmla);
    _add_one_point_boundary_maps(fmla);
    std::cout << fmla.clusters.size() << " simple maps\n";
    std::cout << fmla.irreflexives.size() << " one-point maps\n";

    if (check_for_solution(fabricated_maps)) return true;
    
    std::unordered_set<BoundaryMap> new_maps;
    std::optional<BoundaryMap> solution;
    size_t old_size = fabricated_maps.size();
    size_t new_size = -1;
    int iter = 1;

    while (old_size != new_size) {
        old_size = new_size;

        new_maps = close(fmla);
        if (print_debug) std::cout << "iteration " << iter << " close:   " << new_maps.size() << " maps found\n";
        solution = check_for_solution(new_maps);
        if (solution.has_value()) return true;
        fabricated_maps.insert(new_maps.begin(), new_maps.end());
        new_maps.clear();

        new_maps = join(fmla);
        if (print_debug) std::cout << "iteration " << iter << " join:    " << new_maps.size() << " maps found\n";
        solution = check_for_solution(new_maps);
        if (solution.has_value()) return true;
        fabricated_maps.insert(new_maps.begin(), new_maps.end());
        new_maps.clear();

        new_maps = shuffle(fmla);
        if (print_debug) std::cout << "iteration " << iter << " shuffle: " << new_maps.size() << " maps found\n";
        solution = check_for_solution(new_maps);
        if (solution.has_value()) return true;
        fabricated_maps.insert(new_maps.begin(), new_maps.end());
        new_maps.clear();

        new_size = fabricated_maps.size();

        ++iter;
    }
    
    return false;
}

void BottomUpAlgorithm::_add_simple_boundary_maps(Formula& fmla) {
    for (auto& cluster : fmla.clusters) {
        BoundaryMap new_bm(cluster, fmla.content);
        fabricated_maps.insert(new_bm);
    }
}

void BottomUpAlgorithm::_add_one_point_boundary_maps(Formula& fmla) {
    for (auto& irrflexive : fmla.irreflexives) {
        BoundaryMap new_bm(irrflexive, fmla.content);
        fabricated_maps.insert(new_bm);
    }
}

std::optional<BoundaryMap> BottomUpAlgorithm::check_for_solution(std::unordered_set<BoundaryMap>& maps) {
    for (auto& bm : maps) {
        if (bm.contains_formula && bm.is_open()) return std::make_optional(bm);
    }
    return std::nullopt;
}

std::unordered_set<BoundaryMap> BottomUpAlgorithm::join(Formula& fmla) {
    std::unordered_set<BoundaryMap> result;
    int n = fabricated_maps.size();

    for (auto& bm_a : fabricated_maps) {
        for (auto& bm_b : fabricated_maps) {
            for (int direction = 0; direction < 4; ++direction) {
                std::optional<BoundaryMap> new_bm = _join_with_direction(bm_a, bm_b, direction);
                if (new_bm.has_value()) result.insert(new_bm.value());
            }
        }
    }

    return result;
}

std::optional<BoundaryMap> BottomUpAlgorithm::_join_with_direction(const BoundaryMap& a, const BoundaryMap& b, int direction) {
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
bool BottomUpAlgorithm::_join_check_continuous_edges(const BoundaryMap& a, const BoundaryMap& b, int direction) {
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

// only N and E joins, swap args for S and W joins
// returns std::nullopt if the join fails because joining either two edge traces fails
std::optional<BoundaryMap> BottomUpAlgorithm::_join_create_new_boundary_map(const BoundaryMap& a, const BoundaryMap& b, int direction) {
    BoundaryMap new_bm;

    if (direction == 0) { // N join
        new_bm.l = b.l;
        new_bm.r = a.r;
        new_bm.b = a.b;
        new_bm.t = b.t;
        
        new_bm.n = b.n;
        if (a.e.has_value() && a.t.has_value() && b.e.has_value()) {
            Trace new_e(a.e.value()); 
            if (new_e.push_back(&Element{a.t.value()}) && new_e.join(*b.e.value())) {
                new_bm.e = std::make_optional(&new_e);
            } else {
                return std::nullopt;
            }
        } else {
            new_bm.e = std::nullopt;
        }

        new_bm.s = a.s;
        if (a.w.has_value() && a.l.has_value() && b.w.has_value()) {
            Trace& new_w = *a.w.value(); 
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
    } else if (direction == 1) { // E join
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

    new_bm.contains_formula = a.contains_formula || b.contains_formula;
    return std::make_optional(new_bm);
}

// only checking:
// n -> t (future)
// e -> t (future)
// s -> b (past)
// w -> b (past)
// everything else is "untouched" by the join
bool BottomUpAlgorithm::_join_defects_passed_correctly(const BoundaryMap& a) {
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

void BottomUpAlgorithm::_join_collect_defects_along_trace(const Trace& trace, std::unordered_set<std::string>& defects, bool reverse_time) {
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

/*
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

// exclude boundary maps m, where m <= minus || plus <= m
void BottomUpAlgorithm::_shuffle_guess_array(std::vector<BoundaryMap>& result, Cluster& plus, Cluster& minus, std::vector<BoundaryMap>& arr, int i, int max_arr_size) {
    if (i == max_arr_size) return;

    std::optional<BoundaryMap> new_bm = _shuffle_create_new_boundary_map(plus, minus, arr);
    if (new_bm.has_value()) result.push_back(new_bm.value());

    // if (minus <= fabricated_maps[i].b.value() && fabricated_maps[i] <= plus) {
    // }
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
*/

std::unordered_set<BoundaryMap> BottomUpAlgorithm::shuffle(Formula& fmla) {
    std::unordered_set<BoundaryMap> result;
    int n = fmla.clusters.size();
    // int m = fabricated_maps.size();

    // guess a pair of clusters to be the plus and minus
    for (auto& plus : fmla.clusters) {
        for (auto& minus : fmla.clusters) {
            // exclude clusters pairs in the wrong order
            // also contains the case when a == b
            if (minus <= plus && (minus != plus)) {
                std::optional<BoundaryMap> new_bm = _shuffle_find_shuffle(plus, minus);
                if (new_bm.has_value()) result.insert(new_bm.value());
            } 
            
        }
    }
    
    return result;
}

std::optional<BoundaryMap> BottomUpAlgorithm::_shuffle_find_shuffle(Cluster& plus, Cluster& minus) {
    // skip in future iterations
    // can have a separate unordered_set<pair<int, int>> cache actually
    if (fabricated_maps.contains(_shuffle_create_new_boundary_map(plus, minus, true))) return std::nullopt;

    int m = fabricated_maps.size();
    int i = 0;
    bool found_one_point_map = false;
    bool found_formula = false;
    bool found_shuffle = false;
    std::unordered_set<std::string> minus_future_defects;
    std::unordered_set<std::string> plus_past_defects;
    
    // find defects
    for (auto& fmla : plus.formulas)
        if (fmla[0] == 'P') plus_past_defects.insert(fmla);
    for (auto& fmla : minus.formulas)
        if (fmla[0] == 'F') minus_future_defects.insert(fmla);

    for (auto& current : fabricated_maps) {
        // BM cant be used
        if (!current.is_closed() || !(minus <= current.b.value()) || !(current.t.value() <= plus)) continue;

        if (current.is_one_point()) found_one_point_map = true;
        found_formula |= current.contains_formula;
        
        // no need to search further
        if (found_shuffle && found_formula) break;
        // no more defects to resolve
        if (found_shuffle) continue;
        // havent found a shuffle, resolve remaining defects
        for (auto& fmla : current.t.value().formulas) {
            if (fmla[0] == 'P') {
                if (plus_past_defects.contains(fmla)) plus_past_defects.erase(fmla);
            } else {
                std::string past_fmla = std::string(1, 'P') + fmla;
                if (plus_past_defects.contains(past_fmla)) plus_past_defects.erase(past_fmla);
            }
        }
        for (auto& fmla : current.b.value().formulas) {
            if (fmla[0] == 'F') {
                if (minus_future_defects.contains(fmla)) minus_future_defects.erase(fmla);
            } else {
                std::string future_fmla = std::string(1, 'F') + fmla;
                if (minus_future_defects.contains(future_fmla)) minus_future_defects.erase(future_fmla);
            }
        }

        if (plus_past_defects.empty() && minus_future_defects.empty() && found_one_point_map) found_shuffle = true;
        if (found_shuffle && found_formula) break;
    }
    
    return (found_shuffle) ? std::make_optional(_shuffle_create_new_boundary_map(plus, minus, found_formula)) : std::nullopt;
}

BoundaryMap BottomUpAlgorithm::_shuffle_create_new_boundary_map(Cluster& plus, Cluster& minus, bool contains_formula) {
    BoundaryMap new_bm;
    new_bm.plus = plus;
    new_bm.minus = minus;
    new_bm.contains_formula = contains_formula;
    return new_bm;
}

/* 
corner is added IFF both adj edges are added
:thinking: :thinking: :thinking:

add_edges()
DFS on directions [1..4], skip the ones not in config
at depth == 3, call add_corners()

add_corners()
DFS on corners [1..4], skip the ones that cant be added
at depth == 3, insert the new map to result

or try this
plus and minus are fixed, use those as anchors
find any [0..2] clusters <= minus
find any [0..2] clusters >= plus
temporal ordering should be preserved this way???
*/

std::unordered_set<BoundaryMap> BottomUpAlgorithm::close(Formula& fmla) {
    std::unordered_set<BoundaryMap> new_maps;

    for (auto& bm : fabricated_maps) {
        // ignore particaly closed or fully closed ones
        if (!bm.is_open()) continue;
        _close_add_edges(fmla, bm, new_maps, 0);
    }
    
    return new_maps;
}

// 15 configs for edges
// direction
// 0: n, e
// 1: s, w
void BottomUpAlgorithm::_close_add_edges(Formula& fmla, const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int direction) {
    Element& plus = Element{bm.plus.value()};
    Element& minus = Element{bm.minus.value()};
    
    if (direction == 0) {
        _close_add_edges(fmla, bm, new_maps, 1);

        for (auto& element_n : fmla.suc[plus]) {
            if (std::holds_alternative<MaximalConsistentSet>(element_n)) continue;
            for (auto& element_e : fmla.pre[plus]) {
                if (std::holds_alternative<MaximalConsistentSet>(element_e)) continue;
                if (element_n == element_e) continue;
                
                BoundaryMap new_bm_n(bm);
                BoundaryMap new_bm_e(bm);
                BoundaryMap new_bm_ne(bm);
                Trace n(std::get<Cluster*>(element_n));
                Trace e(std::get<Cluster*>(element_e));
                
                new_bm_n.n = n;
                new_bm_e.e = e;
                new_bm_ne.n = n;
                new_bm_ne.e = e;
                
                _close_add_edges(fmla, new_bm_n, new_maps, 1);
                _close_add_edges(fmla, new_bm_e, new_maps, 1);
                _close_add_edges(fmla, new_bm_ne, new_maps, 1);

                _close_add_corners(fmla, new_bm_n, new_maps, 1);
                _close_add_corners(fmla, new_bm_e, new_maps, 1);
                _close_add_corners(fmla, new_bm_ne, new_maps, 1);
            }       
        }
    } else if (direction == 1) {
        for (auto& element_s : fmla.pre[minus]) {
            if (std::holds_alternative<MaximalConsistentSet>(element_s)) continue;

            for (auto& element_w : fmla.pre[minus]) {
                if (std::holds_alternative<MaximalConsistentSet>(element_w)) continue;
                if (element_s == element_w) continue;

                BoundaryMap new_bm_s(bm);
                BoundaryMap new_bm_w(bm);
                BoundaryMap new_bm_sw(bm);
                Trace s(std::get<Cluster>(element_s));
                Trace w(std::get<Cluster>(element_w));

                new_bm_s.s = s;
                new_bm_w.w = w;
                new_bm_sw.s = s;
                new_bm_sw.w = w;

                _close_add_corners(fmla, new_bm_s, new_maps, 0);
                _close_add_corners(fmla, new_bm_w, new_maps, 0);
                _close_add_corners(fmla, new_bm_sw, new_maps, 0);
            }       
        }
    } else {
        return;
    }
}

void BottomUpAlgorithm::_close_add_corners(Formula& fmla, const BoundaryMap& bm, std::unordered_set<BoundaryMap>& new_maps, int corner) {
    if (corner == 0) { // l
        if (bm.w.has_value() && bm.n.has_value()) {
            std::unordered_set<Formula::Element> candidates(
                fmla.suc[std::get<Cluster>(bm.w.value().sequence[0])].begin(), 
                fmla.suc[std::get<Cluster>(bm.w.value().sequence[0])].end()
            );
            std::unordered_set<Formula::Element> other(
                fmla.pre[std::get<Cluster>(bm.n.value().sequence[0])].begin(), 
                fmla.pre[std::get<Cluster>(bm.n.value().sequence[0])].end()
            );

            for (auto& element : candidates)
                if (!other.contains(element) || std::holds_alternative<Cluster>(element)) candidates.erase(element);

            for (auto& element : candidates) {
                BoundaryMap new_bm(bm);
                new_bm.l = std::get<MaximalConsistentSet>(element);
                _close_add_corners(fmla, new_bm, new_maps, 1);
            }
        } else {
            _close_add_corners(fmla, bm, new_maps, 1);
        }
    } else if (corner == 1) { // r
        if (bm.e.has_value() && bm.s.has_value()) {
            std::unordered_set<Formula::Element> candidates(
                fmla.pre[std::get<Cluster>(bm.e.value().sequence[0])].begin(), 
                fmla.pre[std::get<Cluster>(bm.e.value().sequence[0])].end()
            );
            std::unordered_set<Formula::Element> other(
                fmla.suc[std::get<Cluster>(bm.s.value().sequence[0])].begin(), 
                fmla.suc[std::get<Cluster>(bm.s.value().sequence[0])].end()
            );

            for (auto& element : candidates)
                if (!other.contains(element) || std::holds_alternative<Cluster>(element)) candidates.erase(element);

            for (auto& element : candidates) {
                BoundaryMap new_bm(bm);
                new_bm.r = std::get<MaximalConsistentSet>(element);
                _close_add_corners(fmla, new_bm, new_maps, 2);
            }
        } else {
            _close_add_corners(fmla, bm, new_maps, 2);
        }
    } else if (corner == 2) { // t
        if (bm.n.has_value() && bm.e.has_value()) {
            std::unordered_set<Formula::Element> candidates(
                fmla.suc[std::get<Cluster>(bm.n.value().sequence[0])].begin(), 
                fmla.suc[std::get<Cluster>(bm.n.value().sequence[0])].end()
            );
            std::unordered_set<Formula::Element> other(
                fmla.suc[std::get<Cluster>(bm.e.value().sequence[0])].begin(), 
                fmla.suc[std::get<Cluster>(bm.e.value().sequence[0])].end()
            );

            for (auto& element : candidates) 
                if (!other.contains(element) || std::holds_alternative<Cluster>(element)) candidates.erase(element);
            
            for (auto& element : candidates) {
                BoundaryMap new_bm(bm);
                new_bm.t = std::get<MaximalConsistentSet>(element);
                _close_add_corners(fmla, new_bm, new_maps, 3);
            }
        } else {
            _close_add_corners(fmla, bm, new_maps, 3);
        }
    } else if (corner == 3) { // b
        if (bm.s.has_value() && bm.w.has_value()) {
            std::unordered_set<Formula::Element> candidates(
                fmla.pre[std::get<Cluster>(bm.s.value().sequence[0])].begin(), 
                fmla.pre[std::get<Cluster>(bm.s.value().sequence[0])].end()
            );
            std::unordered_set<Formula::Element> other(
                fmla.pre[std::get<Cluster>(bm.w.value().sequence[0])].begin(), 
                fmla.pre[std::get<Cluster>(bm.w.value().sequence[0])].end()
            );

            for (auto& element : candidates)
                if (!other.contains(element) || std::holds_alternative<Cluster>(element)) candidates.erase(element);

            for (auto& element : candidates) {
                BoundaryMap new_bm(bm);
                new_bm.b = std::get<MaximalConsistentSet>(element);
                new_maps.insert(new_bm);
            }
        } else {
            new_maps.insert(bm);
        }
    } else {
        return;
    }
}