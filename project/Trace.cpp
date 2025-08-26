#include "Trace.h"

using Element = std::variant<MaximalConsistentSet*, Cluster*>;


Trace::Trace() {};

bool Trace::operator==(const Trace& other) const {
    return sequence == other.sequence;
}

// bool Trace::contains_formula(std::string& str) {
//     for (auto& elem : sequence)
//         if (std::holds_alternative<Cluster>(elem) && std::get<Cluster>(elem).formulas.contains(str)
//             || std::holds_alternative<MaximalConsistentSet>(elem) && std::get<MaximalConsistentSet>(elem).formulas.contains(str))
//             return true;
//     return false;
// }

bool Trace::push_back(Element* elem) {
    if (sequence.size() == 0) {
        sequence.push_back(elem);
        return true;
    }

    auto& back = *sequence.back();
    auto& next = *elem;

    bool ok = std::visit([&](auto* lastPtr, auto* nextPtr) -> bool {
        using LastT = std::decay_t<decltype(*lastPtr)>;
        using NextT = std::decay_t<decltype(*nextPtr)>;

        // only ok if different types, then check temporal ordering
        if constexpr (!std::is_same_v<LastT, NextT>) {
            return (*lastPtr <= *nextPtr);
        } else {
            return false;
        }
    }, back, next);

    if (ok) {
        sequence.push_back(elem);
        return true;
    }
    return false;
}

bool Trace::join(Trace& other) {
    if (sequence.size() == 0 || other.sequence.size() == 0) return false;

    auto& back  = *sequence.back();
    auto& front = *other.sequence.front();

    bool ok = std::visit([&](auto* lastPtr, auto* nextPtr) -> bool {
        using LastT = std::decay_t<decltype(*lastPtr)>;
        using NextT = std::decay_t<decltype(*nextPtr)>;

        // if same type, then they must be equal
        // if not,       then they must follow ordering
        if constexpr (std::is_same_v<LastT, NextT>) {
            return (*lastPtr == *nextPtr);
        } else {
            return (*lastPtr <= *nextPtr);
        }
    }, back, front);

    if (!ok) return false;

    // do the join
    // if the back and head are equal that's fine, the first Trace::push_back() will fail and we ignore it
    for (Element* elem : other.sequence) push_back(elem);

    return true;
}

void Trace::show_sequence() {
    std::cout << "sequence length: " << sequence.size() << " elements\n";
    int i = 0;
    for (Element* elem : sequence) {
        std::cout << i << ". ";
        if (std::holds_alternative<MaximalConsistentSet*>(*elem)) {
            std::cout << "mcs: ";
            std::get<MaximalConsistentSet*>(*elem)->show_formulas();
        } else if (std::holds_alternative<Cluster*>(*elem)) {
            std::cout << "cluster: ";
            std::get<Cluster*>(*elem)->show_formulas();
        }
    }
}

int Trace::size() {
    return sequence.size();
}
