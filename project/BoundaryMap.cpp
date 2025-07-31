#include "BoundaryMap.h"
 

BoundaryMap::BoundaryMap() {}

BoundaryMap::BoundaryMap(Cluster& cluster, std::string& fmla) : contains_formula(cluster.formulas.contains(fmla)), plus(cluster), minus(cluster) {}

bool BoundaryMap::is_simple() {
    return plus.has_value() && minus.has_value() && plus == minus;
}

bool BoundaryMap::is_open() {
    // TODO: partially open counts right? this code below is for fully open ones
    return !l.has_value() && !r.has_value() && !t.has_value() && !b.has_value() 
        && !n.has_value() && !e.has_value() && !s.has_value() && !w.has_value() 
        && plus.has_value() && minus.has_value();
        // TODO: plus and minus cannot have defects, write a function to check that
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

bool BoundaryMap::has_internal_defects() {
    // go forward in time, collect defects, remove them if resolved?
    // same the other way round
    std::unordered_set<std::string> past_defects;
    std::unordered_set<std::string> future_defects;

    return true;
}

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
