#include "Formula.h"
#include "BoundaryMap.h"
 

BoundaryMap::BoundaryMap()
 : contains_formula(false) {}

BoundaryMap::BoundaryMap(Cluster* cluster, std::string& fmla)
 : contains_formula(cluster->formulas.contains(cluster->parent.string_to_id[fmla])), plus(cluster), minus(cluster) {}

BoundaryMap::BoundaryMap(MaximalConsistentSet* irreflexive, std::string& fmla)
 : contains_formula(irreflexive->formulas.contains(irreflexive->parent.string_to_id[fmla])), t(irreflexive), b(irreflexive) {}

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
        && plus.has_value() && minus.has_value() ||
        is_one_point();
}

// {t, b}, t == b
bool BoundaryMap::is_one_point() const {
    return !l.has_value() && !r.has_value() && t.has_value() && b.has_value() 
        && !n.has_value() && !e.has_value() && !s.has_value() && !w.has_value() 
        && !plus.has_value() && !minus.has_value()
        && t.value() == b.value();
}
