#include "BoundaryMap.h"
 

BoundaryMap::BoundaryMap() {}

BoundaryMap::BoundaryMap(Cluster& cluster) : plus(cluster), minus(cluster) {}

bool BoundaryMap::is_simple() {
    return plus.has_value() && minus.has_value() && plus == minus;
}

bool BoundaryMap::is_closed() {
    return l.has_value() && r.has_value() && t.has_value() && b.has_value() && n.has_value() && e.has_value() && s.has_value() && w.has_value() && plus.has_value() && minus.has_value();
}

bool BoundaryMap::is_fabricated() {
    // TODO: the main algorithm lmao
    if (is_simple()) return true;

    return false;
}

bool BoundaryMap::join(BoundaryMap& other, int direction) {
    bool success = false;

    if (direction == 0) { // N

    } else if (direction == 1) { // E

    } else if (direction == 2) { // S

    } else if (direction == 3) { // W

    } else { // invalid
        return false;
    }

    return success;
}
