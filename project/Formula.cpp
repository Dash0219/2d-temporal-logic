#include "Formula.h"


bool Formula::setup_formula(std::string& input) {
    content = input;
    if (!parse_and_build_tree(input, root)) return false;
    generate_closure_set();
    generate_MCS_and_clusters();
    return true;
}

void Formula::clear() {
    content = "";
    propositions.clear();
    temporal_formulas.clear();
    closure_set.clear();
    MCS.clear();
    clear_node(root);
}

void Formula::show_tree() {
    if (!root) return;
    std::stack<std::pair<ASTNode*, int>> st;
    st.push({root, 0});

    std::cout << "AST of " << content << ":\n";
    while (!st.empty()) {
        auto [node, depth] = st.top();
        st.pop();
        std::cout << std::string(depth * 2, ' ') << "|-" << node->content << "\n";
        if (node->right) st.push({node->right, depth + 1});
        if (node->left) st.push({node->left, depth + 1});
    }
    std::cout << std::string(32, '=') << "\n";
}

void Formula::show_propositions() {
    std::cout << "propositions of " << content << ": " << propositions.size() << " elements\n";
    for (const std::string& p : propositions) std::cout << p << "\n";
    std::cout << std::string(32, '=') << "\n";
}

void Formula::show_temporal_formulas() {
    std::cout << "temporal formulas of " << content << ": " << temporal_formulas.size() << " elements\n";
    for (const std::string& f : temporal_formulas) std::cout << f << "\n";
    std::cout << std::string(32, '=') << "\n";
}

void Formula::show_closure_set() {
    std::cout << "closure set of " << content << ": " << closure_set.size() << " elements\n";
    for (const std::string& f : closure_set) std::cout << f << "\n";
    std::cout << std::string(32, '=') << "\n";
}

void Formula::show_MCS() {
    std::cout << "MCS of " << content << ": " << MCS.size() << " elements\n";
    for (const MaximalConsistentSet& mcs : MCS) {
        std::cout << "{";
        bool first = true;
        for (const std::string& f : mcs.formulas) {
            if (!first) std::cout << ", ";
            std::cout << f;
            first = false;
        }
        std::cout << "}\n";
    }
    std::cout << std::string(32, '=') << "\n";
}

void Formula::show_clusters() {
    std::cout << "clusters of " << content << ": " << clusters.size() << " elements\n";
    for (const Cluster& cluster : clusters) {
        std::cout << "{";
        bool first = true;
        for (const std::string& f : cluster.formulas) {
            if (!first) std::cout << ", ";
            std::cout << f;
            first = false;
        }
        std::cout << "}\n";

        std::cout << "representative: {";
        first = true;
        for (const std::string& f : cluster.representative.formulas) {
            if (!first) std::cout << ", ";
            std::cout << f;
            first = false;
        }
        std::cout << "}\n";
    }


    std::cout << std::string(32, '=') << "\n";
}

Formula::~Formula() {
    clear();
}

bool Formula::is_binary_op(const std::string& input, int pos) {
    return (pos + 1 < input.size() &&
           ((input[pos] == '/' && input[pos + 1] == '\\') ||
            (input[pos] == '\\' && input[pos + 1] == '/') ||
            (input[pos] == '-' && input[pos + 1] == '>') ||
            (input[pos] == '<' && input[pos + 1] == '>')));
}

bool Formula::parse_and_build_tree(std::string& input, ASTNode*& node) {
    if (input.empty()) return false;
    node = new ASTNode();

    if (input[0] == '~' || input[0] == 'G' || input[0] == 'H' || input[0] == 'F' || input[0] == 'P') {
        std::string subfmla = input.substr(1);
        ASTNode* child = nullptr;
        if (!parse_and_build_tree(subfmla, child)) return false;
        node->left = child;
        node->content = std::string(1, input[0]);
        if (input[0] == '~') {
            closure_set.insert(input);
        } else {
            temporal_formulas.insert(input), closure_set.insert(input);
        }
        return true;
    } else if (input[0] == '(') {
        if (input.back() != ')') return false;

        int n = input.size();
        int depth = 0;
        int operator_pos = -1;
        for (int i = 1; i < n - 1; ++i) {
            if (input[i] == '(') {
                ++depth;
            } else if (input[i] == ')') {
                --depth;
            } else if (depth == 0 && is_binary_op(input, i)) {
                operator_pos = i;
                break;
            }
            if (depth < 0) return false;
        }
        if (operator_pos == -1 || depth != 0) return false;

        std::string left_subfmla = input.substr(1, operator_pos - 1);
        ASTNode* left_child = nullptr;
        if (!parse_and_build_tree(left_subfmla, left_child)) return false;
        node->left = left_child;

        std::string right_subfmla = input.substr(operator_pos + 2, n - operator_pos - 3);
        ASTNode* right_child = nullptr;
        if (!parse_and_build_tree(right_subfmla, right_child)) return false;
        node->right = right_child;

        node->content = input.substr(operator_pos, 2);
        closure_set.insert(input);
        return true;
    } else {
        for (char c : input) if (!std::islower(c) && !std::isdigit(c) && c != '_') return false;
        node->content = input;
        propositions.insert(input);
        closure_set.insert(input);
        return true;
    }
    return false;
}

void Formula::generate_closure_set() {
    std::unordered_set<std::string> new_members;
    for (const std::string& fmla : closure_set)
        new_members.insert("~" + fmla);
    closure_set.insert(new_members.begin(), new_members.end());
}

void Formula::generate_MCS_and_clusters() {
    std::vector<std::string> props_and_temps(propositions.begin(), propositions.end());
    std::unordered_set<std::string> true_bases;

    for (const std::string& t : temporal_formulas) props_and_temps.push_back(t);
    _generate_MCS_step(props_and_temps, true_bases, 0);
}

void Formula::_generate_MCS_step(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases, int current) {
    if (current == props_and_temps.size()) {
        _get_mcs_from_valuations(props_and_temps, true_bases);
        return;
    }
    _generate_MCS_step(props_and_temps, true_bases, current + 1);
    true_bases.insert(props_and_temps[current]);
    _generate_MCS_step(props_and_temps, true_bases, current + 1);
    true_bases.erase(props_and_temps[current]);
}

void Formula::_get_mcs_from_valuations(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases) {
    MaximalConsistentSet mcs(closure_set);
    std::unordered_map<std::string, bool> cache;
    for (const std::string& fmla : closure_set)
        if (_evaluate(const_cast<std::string&>(fmla), true_bases, cache))
            mcs.formulas.insert(fmla);
    
    MCS.push_back(mcs);

    // check all the existing clusters,
    // add the new mcs to a cluster if possible
    bool merged = false;
    for (auto& cluster : clusters) {
        if (cluster.representative <= mcs && mcs <= cluster.representative) {
            for (std::string fmla : mcs.formulas)
                cluster.formulas.insert(fmla);
            merged = true;
            break;
        }
    }

    if (!merged) {
        // create a new singleton cluster
        std::cout << "HIII\n";
        MaximalConsistentSet& last_mcs = MCS.back();
        Cluster new_cluster(last_mcs);
        std::cout << "HUHHH\n";
        clusters.push_back(new_cluster);
    }
}

bool Formula::_evaluate(std::string& fmla, std::unordered_set<std::string>& true_bases, std::unordered_map<std::string, bool>& cache) {
    if (fmla.empty()) return false;
    if (true_bases.contains(fmla)) {
        cache[fmla] = true;
        return true;
    }
    if (cache.contains(fmla)) return cache[fmla];

    bool result = false;
    if (fmla[0] == '~') {
        std::string sub = fmla.substr(1);
        result = !_evaluate(sub, true_bases, cache);
    } else if (fmla[0] == '(' && fmla.back() == ')') {
        int n = fmla.size();
        int depth = 0;
        int op = -1;
        for (int i = 1; i < n - 1; ++i) {
            if (fmla[i] == '(') {
                ++depth;
            } else if (fmla[i] == ')') {
                --depth;
            } else if (depth == 0 && is_binary_op(fmla, i)) {
                op = i;
                break;
            }
        }
        if (op == -1 || depth != 0) return false;

        std::string l = fmla.substr(1, op - 1);
        std::string r = fmla.substr(op + 2, n - op - 3);
        if (fmla[op] == '/' && fmla[op + 1] == '\\') {
            result = _evaluate(l, true_bases, cache) && _evaluate(r, true_bases, cache);
        } else if (fmla[op] == '\\' && fmla[op + 1] == '/') {
            result = _evaluate(l, true_bases, cache) || _evaluate(r, true_bases, cache);
        } else if (fmla[op] == '-' && fmla[op + 1] == '>') {
            result = !_evaluate(l, true_bases, cache) || _evaluate(r, true_bases, cache);
        } else if (fmla[op] == '<' && fmla[op + 1] == '>') {
            bool lv = _evaluate(l, true_bases, cache);
            bool rv = _evaluate(r, true_bases, cache);
            result = (lv && rv) || (!lv && !rv);
        }
    }
    cache[fmla] = result;
    return result;
}

void Formula::clear_node(ASTNode*& node) {
    if (!node) return;
    clear_node(node->left);
    clear_node(node->right);
    delete node;
    node = nullptr;
}