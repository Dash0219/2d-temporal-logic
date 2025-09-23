#include "Formula.h"
#include "Cluster.h"
#include "MaximalConsistentSet.h"


MaximalConsistentSet::MaximalConsistentSet(Formula& parent)
    : parent(parent) {}

bool MaximalConsistentSet::operator==(const MaximalConsistentSet& other) const {
    // for (auto& fmla : formulas)       if (!other.formulas.contains(fmla)) return false;
    // for (auto& fmla : other.formulas) if (!formulas.contains(fmla))       return false;
    // return true;
    return formulas == other.formulas;
}

bool MaximalConsistentSet::operator<=(const MaximalConsistentSet& other) const {
    int closure_set_size = parent.closure_set.size();
    for (int id = 0; id < closure_set_size; ++id) {
        std::string fmla = parent.id_to_string[id];
        std::string phi = fmla.substr(1);
        int phi_id = (parent.string_to_id.contains(phi)) ? parent.string_to_id[phi] : -1;

        if (fmla[0] == 'F') {
            if (other.formulas.contains(phi_id) && !formulas.contains(id) ||
                other.formulas.contains(id) && !formulas.contains(id)) return false;
        } else if (fmla[0] == 'P') {
            if (formulas.contains(phi_id) && !other.formulas.contains(id) ||
                formulas.contains(id) && !other.formulas.contains(id)) return false;
        } else if (fmla[0] == 'G') {
            if (formulas.contains(id) && !other.formulas.contains(phi_id) ||
                formulas.contains(id) && !other.formulas.contains(id)) return false;
        } else if (fmla[0] == 'H') {
            if (other.formulas.contains(id) && !formulas.contains(phi_id) ||
                other.formulas.contains(id) && !formulas.contains(id)) return false;
        } 

        // else if (fmla[0] == '~' && fmla.size() > 2) {
        //     std::string negated_phi = std::string(1, '~') + fmla.substr(2); 
        //     if (fmla[1] == 'F') {
                
        //     } else if (fmla[1] == 'P') {

        //     } else if (fmla[1] == 'G') {

        //     } else if (fmla[1] == 'H') {

        //     }
        // }
    }
    return true;
}

bool MaximalConsistentSet::operator<=(const Cluster& other) const {
    return *this <= *other.representative;
}

void MaximalConsistentSet::show_formulas() const {
    std::cout << "{";
    bool first = true;
    for (int id : formulas) {
        const std::string& f = parent.id_to_string[id];
        if (!first) std::cout << ", ";
        std::cout << f;
        first = false;
    }
    std::cout << "}\n";
}

// bool MaximalConsistentSet::_compare_identity(const MaximalConsistentSet& other, const std::string& fmla) const {
//     std::string phi = fmla.substr(1);

//     if (fmla[0] == 'F') {
//         if (other.formulas.contains(phi) && !formulas.contains(fmla) ||
//             other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'P') {
//         if (formulas.contains(phi) && !other.formulas.contains(fmla) ||
//             formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'G') {
//         if (formulas.contains(fmla) && !other.formulas.contains(phi) ||
//             formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'H') {
//         if (other.formulas.contains(fmla) && !formulas.contains(phi) ||
//             other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
//     }

//     return true;
// }
