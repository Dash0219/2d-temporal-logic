#include "BoundaryMap.h"
 

BoundaryMap::BoundaryMap() : contains_formula(false) {}

BoundaryMap::BoundaryMap(Cluster* cluster, std::string& fmla) : contains_formula(cluster->formulas.contains(fmla)), plus(cluster), minus(cluster) {}

BoundaryMap::BoundaryMap(MaximalConsistentSet* irreflexive, std::string& fmla) : contains_formula(irreflexive->formulas.contains(fmla)), t(irreflexive), b(irreflexive) {}

bool BoundaryMap::operator==(const BoundaryMap& other) const {
    return contains_formula == other.contains_formula &&
           l == other.l && r == other.r && t == other.t && b == other.b &&
           n == other.n && e == other.e && s == other.s && w == other.w &&
           plus == other.plus && minus == other.minus;
}

// {+, -}, + == -
bool BoundaryMap::is_simple() const {
    return is_open() && plus == minus;
}

// {+, -}
bool BoundaryMap::is_open() const {
    return !l.has_value() && !r.has_value() && !t.has_value() && !b.has_value() 
        && !n.has_value() && !e.has_value() && !s.has_value() && !w.has_value() 
        && plus.has_value() && minus.has_value();
}

// {l, r, t, b, n, e, s, w, +, -}
bool BoundaryMap::is_closed() const {
    return l.has_value() && r.has_value() && t.has_value() && b.has_value() 
        && n.has_value() && e.has_value() && s.has_value() && w.has_value() 
        && plus.has_value() && minus.has_value();
}

// {t, b}, t == b
bool BoundaryMap::is_one_point() const {
    return !l.has_value() && !r.has_value() && t.has_value() && b.has_value() 
        && !n.has_value() && !e.has_value() && !s.has_value() && !w.has_value() 
        && !plus.has_value() && !minus.has_value()
        && t.value() == b.value();
}

// bool BoundaryMap::is_fabricated() {
//     // TODO: the main algorithm lmao
//     if (is_simple()) {
//         return true;
//     } else {

//     }

//     return false;
// }

// bool BoundaryMap::contains_formula(std::string& str) {
//     return (l.has_value()     && l.value().formulas.contains(str) 
//          || r.has_value()     && r.value().formulas.contains(str) 
//          || t.has_value()     && t.value().formulas.contains(str) 
//          || b.has_value()     && b.value().formulas.contains(str) 
//          || n.has_value()     && n.value().contains_formula(str)
//          || e.has_value()     && e.value().contains_formula(str)
//          || s.has_value()     && s.value().contains_formula(str)
//          || w.has_value()     && w.value().contains_formula(str)
//          || plus.has_value()  && plus.value().formulas.contains(str)
//          || minus.has_value() && minus.value().formulas.contains(str));
// }

// TODO: i added something like this in BottomUpAlgorithm::join()
// bool BoundaryMap::has_internal_defects() {
//     // go forward in time, collect defects, remove them if resolved?
//     // same the other way round
//     std::unordered_set<std::string> past_defects;
//     std::unordered_set<std::string> future_defects;

//     return true;
// }

// bool BoundaryMap::join(BoundaryMap& other, int direction) {
//     bool success = false;

//     if (direction == 0) { // N

//     } else if (direction == 1) { // E

//     } else if (direction == 2) { // S

//     } else if (direction == 3) { // W

//     } else { // invalid
//         return false;
//     }

//     return success;
// }
