#include <cassert>
#include <iostream>
#include <stack>
#include <string>
#include <sys/resource.h>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include "Formula.h"
#include "ASTNode.h"
#include "MaximalConsistentSet.h"
#include "Cluster.h"
#include "Trace.h"
#include "BottomUpAlgorithm.h"


/*
grammar:
FMLA := (FMLA binary FMLA) | unary FMLA | prop
binary := /\ | \/ | -> | <>
unary := ~ | G | H | F | P
prop := [a..z0..9_]+
*/
/*`
class ASTNode {
public:
    string content;
    ASTNode *left;
    ASTNode *right;

    ASTNode() {
        content = ""; 
        left = nullptr; 
        right = nullptr;
    }
    
    ASTNode(string c) {
        content = c; 
        left = nullptr; 
        right = nullptr;
    }
};


class Formula {
public: 
    string content;
    unordered_set<string> propositions;
    unordered_set<string> temporal_formulas;
    unordered_set<string> closure_set;
    // vector<unordered_set<string>> MCS;
    vector<MaximalConsistentSet> MCS;
    
    bool setup_formula(string& input) {
        content = input;
        if (!parse_and_build_tree(input, root)) return false;
        generate_closure_set();
        generate_MCS();
        return true;
    }

    void clear() {
        content = "";
        propositions.clear();
        temporal_formulas.clear();
        closure_set.clear();
        MCS.clear();
        clear_node(root);
    }

    void show_tree() {
        if (!root) return;
    
        stack<pair<ASTNode*, int>> st;
        st.push({root, 0});
    
        cout << "AST of " << content << ": " << endl;

        while (!st.empty()) {
            auto [node, depth] = st.top();
            st.pop();
            
            cout << string(depth * 2, ' ') << "|-" << node->content << endl;
            
            if (node->right) st.push({node->right, depth + 1});
            if (node->left)  st.push({node->left, depth + 1});
        }

        cout << string(32, '=') << endl;
    }

    void show_propositions() {
        cout << "propositions of " << content << ": " << propositions.size() << " elements" << endl;
        for (string p : propositions) cout << p << endl;
        cout << string(32, '=') << endl;
    }

    void show_temporal_formulas() {
        cout << "temporal formulas of " << content << ": " << temporal_formulas.size() << " elements" << endl;
        for (string f : temporal_formulas) cout << f << endl;
        cout << string(32, '=') << endl;
    }

    // void show_sub_formulas() {
    //     cout << "sub formulas of " << content << ": " << endl;
    //     for (string f : sub_formulas) cout << f << endl;
    // }
    
    void show_closure_set() {
        cout << "closure set of " << content << ": " << closure_set.size() << " elements" << endl;
        for (string f : closure_set) cout << f << endl;
        cout << string(32, '=') << endl;
    }

    void show_MCS() {
        cout << "MCS of " << content << ": " << MCS.size() << " elements" << endl;
        for (MaximalConsistentSet& mcs : MCS) {
            unordered_set<string> cs = mcs.formulas;
            cout << "{";
            bool first = true;
            for (string f : cs) {
                if (!first) cout << ", ";
                cout << f;
                first = false;
            }
            cout << "}" << endl;
        }
        cout << string(32, '=') << endl;
    }
    
private:
    ASTNode *root;

    bool is_binary_op(const string& input, int pos) {
        return (pos + 1 < input.size() &&
               ((input[pos] == '/' && input[pos + 1] == '\\') ||
                (input[pos] == '\\' && input[pos + 1] == '/') ||
                (input[pos] == '-' && input[pos + 1] == '>') ||
                (input[pos] == '<' && input[pos + 1] == '>')));
    }

    bool parse_and_build_tree(string& input, ASTNode*& node) {
        if (input.size() == 0) return false;

        node = new ASTNode();
        
        if (input[0] == '~' || input[0] == 'G' || input[0] == 'H' || input[0] == 'F' || input[0] == 'P') {
            string subfmla = input.substr(1);
            ASTNode* child = nullptr;
            if (!parse_and_build_tree(subfmla, child)) return false;
            node->left = child;

            node->content = string(1, input[0]);

            if (input[0] == '~') {
                closure_set.insert(input);
            } else {
                temporal_formulas.insert(input);
                closure_set.insert(input);
            }
            return true;
        } else if (input[0] == '(') {
            if (input.back() != ')') return false;

            int n = input.size();
            int depth = 0;
            int operator_pos = -1;
            bool found = false;

            for (int i = 1; i < n - 1; ++i) {
                if (input[i] == '(') {
                    ++depth;
                } else if (input[i] == ')') {
                    --depth;
                } else if (depth == 0 && is_binary_op(input, i)) {
                    operator_pos = i;
                    found = true;
                }
                if (depth < 0) return false;
            }
            if (!found || depth != 0) return false;

            string left_subfmla = input.substr(1, operator_pos - 1);
            ASTNode* left_child = nullptr;
            if (!parse_and_build_tree(left_subfmla, left_child)) return false;
            node->left = left_child;
            
            string right_subfmla = input.substr(operator_pos + 2, n - operator_pos - 3);
            ASTNode* right_child = nullptr;
            if (!parse_and_build_tree(right_subfmla, right_child)) return false;
            node->right = right_child;

            node->content = input.substr(operator_pos, 2);
            
            closure_set.insert(input);
            return true;
        } else {
            for (char c : input) if (!islower(c) && !isdigit(c) && c != '_') return false;
            
            node->content = input;
            
            propositions.insert(input);
            closure_set.insert(input);
            return true;
        }

        return false;
    }

    void generate_closure_set() {
        unordered_set<string> new_members;
        for (string fmla : closure_set) 
            new_members.insert(string(1, '~') + fmla);

        closure_set.insert(new_members.begin(), new_members.end());
    }

    void generate_MCS() {
        vector<string> props_and_temps(propositions.begin(), propositions.end());
        unordered_set<string> true_bases;
        
        for (auto it = temporal_formulas.begin(); it != temporal_formulas.end(); ++it) props_and_temps.push_back(*it);
        _step(props_and_temps, true_bases, 0);
    }

    void _step(vector<string>& props_and_temps, unordered_set<string>& true_bases, int current) {
        if (current == props_and_temps.size()) {
            _get_mcs_from_valuations(props_and_temps, true_bases);
            return;
        }

        _step(props_and_temps, true_bases, current + 1);
        true_bases.insert(props_and_temps[current]);
        _step(props_and_temps, true_bases, current + 1);
        true_bases.erase(props_and_temps[current]);
    }

    void _get_mcs_from_valuations(vector<string>& props_and_temps, unordered_set<string>& true_bases) {
        MaximalConsistentSet mcs(this);

        unordered_map<string, bool> cache;
        for (string fmla : closure_set) 
            if (_evaluate(fmla, true_bases, cache)) mcs.formulas.insert(fmla);
        
        MCS.push_back(mcs);
    }

    bool _evaluate(string& fmla, unordered_set<string>& true_bases, unordered_map<string, bool>& cache) {
        if (fmla.size() == 0) return false;
        if (true_bases.contains(fmla)) {
            cache[fmla] = true;
            return true;
        }
        if (cache.contains(fmla)) return cache[fmla];

        bool result;
        if (fmla[0] == '~') {
            string substr = fmla.substr(1);
            result = !_evaluate(substr, true_bases, cache);
        } else if (fmla[0] == '(' && fmla.back() ==')') {
            int depth = 0;
            int n = fmla.size();
            int operator_pos = -1;
            for (int i = 1; i < n - 1; ++i) {
                if (fmla[i] == '(') {
                    ++depth;
                } else if (fmla[i] == ')') {
                    --depth;
                } else if (depth == 0 && is_binary_op(fmla, i)) {
                    operator_pos = i;
                }
            }

            string left_subfmla = fmla.substr(1, operator_pos - 1);
            string right_subfmla = fmla.substr(operator_pos + 2, n - operator_pos - 3);
            if (fmla[operator_pos] == '/' && fmla[operator_pos + 1] == '\\') {
                result = _evaluate(left_subfmla, true_bases, cache) && _evaluate(right_subfmla, true_bases, cache);
            } else if (fmla[operator_pos] == '\\' && fmla[operator_pos + 1] == '/') {
                result = _evaluate(left_subfmla, true_bases, cache) || _evaluate(right_subfmla, true_bases, cache);
            } else if (fmla[operator_pos] == '-' && fmla[operator_pos + 1] == '>') {
                result = !_evaluate(left_subfmla, true_bases, cache) || _evaluate(right_subfmla, true_bases, cache);
            } else if (fmla[operator_pos] == '<' && fmla[operator_pos + 1] == '>') {
                bool l = _evaluate(left_subfmla, true_bases, cache);
                bool r = _evaluate(right_subfmla, true_bases, cache);
                result = l && r || !l && !r;
            }
        }

        cache[fmla] = result;
        return result;
    }

    void clear_node(ASTNode*& node) {
        if (!node) return;

        clear_node(node->left);
        clear_node(node->right);
        delete node;
        node = nullptr;
    }

};


class MaximalConsistentSet {
public:
    std::unordered_set<std::string> formulas;
    const Formula* parent_formula;  // pointer to the formula this MCS belongs to

    MaximalConsistentSet(Formula* f) : parent_formula(f) {

    };

    bool operator<=(const MaximalConsistentSet& other) const {
        // TODO: which definition to use for <=, the one in the paper or the one with G H
        // the one from the paper for m <= n

        // for (const string& fmla : *closure_set) {
        for (const string& fmla : parent_formula->closure_set) {
            if (fmla[0] == 'F') {
                string phi = fmla.substr(1);
                if (other.formulas.contains(phi) && !formulas.contains(fmla)) return false;
                if (other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
            } else if (fmla[0] == 'P') {
                string phi = fmla.substr(1);
                if (formulas.contains(phi) && !other.formulas.contains(fmla)) return false;
                if (formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
            }
        }
        return true;
    }
    // TODO: make the Formula class a global? global pointer to a Formula object? 
    // you won't change it anyway after its initialization

};

*/

///*
// make the graph 
// formatted for: https://csacademy.com/app/graph_editor/
std::vector<std::vector<bool>> create_and_print_graph(Formula& fmla, bool print_non_reduced_graph) {
    int size_i = fmla.irreflexives.size();
    int size_c = fmla.clusters.size();
    std::vector<std::vector<bool>> g(size_i + size_c, std::vector<bool>(size_i + size_c, false));

    int i = 0;
    for (auto it_i = fmla.irreflexives.begin(); it_i != fmla.irreflexives.end(); ++it_i, ++i) {
        int j = 0;
        for (auto it_j = fmla.irreflexives.begin(); it_j != fmla.irreflexives.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[i][j] = true;
                if (print_non_reduced_graph) std::cout << "i" << i << " " << "i" << j << "\n";
            }
        }

        j = 0;
        for (auto it_j = fmla.clusters.begin(); it_j != fmla.clusters.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[i][size_i + j] = true;
                if (print_non_reduced_graph) std::cout << "i" << i << " " << "c" << j << "\n";
            }
        }
    }

    i = 0;
    for (auto it_i = fmla.clusters.begin(); it_i != fmla.clusters.end(); ++it_i, ++i) {
        int j = 0;
        for (auto it_j = fmla.irreflexives.begin(); it_j != fmla.irreflexives.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[size_i + i][j] = true;
                if (print_non_reduced_graph) std::cout << "c" << i << " " << "i" << j << "\n";
            }
        }

        j = 0;
        for (auto it_j = fmla.clusters.begin(); it_j != fmla.clusters.end(); ++it_j, ++j) {
            if (**it_i <= **it_j) {
                g[size_i + i][size_i + j] = true;
                if (print_non_reduced_graph) std::cout << "c" << i << " " << "c" << j << "\n";
            }
        }
    }

    if (print_non_reduced_graph) std::cout << std::string(32, '=') << "\n";

    // trasitive reduction
    for (int i = 0; i < size_i + size_c; ++i) 
        g[i][i] = false;

    std::vector<std::vector<bool>> original = g;
    for (int i = 0; i < size_i + size_c; ++i) {
        for (int j = 0; j < size_i + size_c; ++j) {
            // if (!g[i][j]) continue;
            for (int k = 0; k < size_i + size_c; ++k) {
                // if (g[j][k]) g[i][k] = false;
                if (original[i][j] && original[j][k]) g[i][k] = false;
            }
        }
    }

    for (int i = 0; i < size_i + size_c; ++i) {
        for (int j = 0; j < size_i + size_c; ++j) {
            if (!g[i][j]) continue;
            std::string node_a;
            std::string node_b;
            (i < size_i) ? node_a = "i" + std::to_string(i) : node_a = "c" + std::to_string(i - size_i);
            (j < size_i) ? node_b = "i" + std::to_string(j) : node_b = "c" + std::to_string(j - size_i);
            std::cout << node_a << " " << node_b << "\n";
        }
    }

    std::cout << std::string(32, '=') << "\n";

    return g;
}
//*/

///*
// use (F(p/\q)/\Pr) as the testcase
void trace_test(Formula& fmla) {
    std::cout << std::boolalpha;

    Trace a, b, c, d, e, f, g;
    a.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    a.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    a.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    b.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);
    c.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    c.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    d.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    d.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    e.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    e.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);
    f.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 1)]);
    f.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 3)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.irreflexives.begin(), 1)]);
    g.push_back(fmla.pointer_to_element[*std::next(fmla.clusters.begin(), 2)]);

    a.join(b);
    b.join(a);
    c.join(b);
    d.join(e);
    f.join(g);

    assert(a.sequence.size() == 5);
    assert(b.sequence.size() == 3);
    assert(c.sequence.size() == 5);
    assert(d.sequence.size() == 2);
    assert(e.sequence.size() == 2);
    assert(f.sequence.size() == 5);
    assert(g.sequence.size() == 4);

    assert(a.join(c) == false);
    assert(a == c);
}
//*/

void print_memory_usage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    std::cout << "Max memory used: " << usage.ru_maxrss / 1024 << " MB" << "\n";
}

int main() {
    std::vector<std::string> test_inputs = {
        // "~(p->p)",
        // "~(p->(q->p))",
        // "(p\\/r)",
        // "~((p->p)->(q->q))",
        // "(q/\\~(q/\\(p/\\q)))",

        // "~((p0->F~(p1/\\p2))->(Pp3->Pp3))",

        // "((p0->F~(p1/\\p2))->(Pq->Pq))",
        // "(~((p0->~(p1/\\p2))->(p3->p4))/\\~((p5->~(p6/\\p7))->(p8->p9)))",
        // "(~((p0->~(p1/\\p2))->(p3->p4))/\\~((p5->~(p6/\\p7))->(p8->Pp9)))",

        // "(~((p0->~(p1/\\Fp2))->(Fp3->Fp4))/\\~((p2->~(p0/\\Fp1))->(p2->p0)))",
        // "~~~~Fp", 
        // "P(Fp\\/(Pp\\/p))",
        // "(Fp\\/(Pp\\/p))",
        // "(p/\\F(q->Pp))",
        // "(p/\\~p)",
        // "(F(p/\\q)/\\Pr)",
        // "(~F~p/\\p)"
        // "(Pp/\\~Fp)",
        // "(~Fp/\\~F~p)",
        // "(Fp/\\Gp)",
        // "Pp",
        // "PPPPPPPPPp", // 10
        "PPPPPPPPPPPPPPp", // 15
        // "HHHHHHHHHHHHHHHHHHHp", // 20
    };

    for (std::string test : test_inputs) {
        std::cout << std::string(32, '=') << "\n";
        std::cout << test << " ";
        Formula fmla(test);
        if (fmla.setup_formula()) {
            std::cout << "is a formula" << "\n";
            std::cout << std::string(32, '=') << "\n";
            // fmla.show_tree();
            // fmla.show_propositions();
            // fmla.show_temporal_formulas();
            // fmla.show_closure_set();

            // fmla.show_irreflexives();
            // fmla.show_clusters(true);
            // fmla.show_graphs();

            BottomUpAlgorithm algo;
            if (algo.run(fmla, true)) {
                std::cout << "SAT\n";
            } else {
                std::cout << "not SAT\n";
            }
            std::cout << std::string(32, '=') << "\n";
        } else {
            std::cout << "is not a formula" << "\n";
            std::cout << std::string(32, '=') << "\n";
        }

        // debugging functions
        // std::vector<std::vector<bool>> g;
        // g = create_and_print_graph(fmla, false);
        // trace_test(fmla);
        // print_memory_usage();
    }

    return 0;
}


/*
draft paper:

what is the number of distinct boundary maps?
paper says it's O(exp(n))

4 mcs * 4 trace * 2 cluster
O(exp(n)) * O(exp(n)) * O(exp(n)) = O(exp(n))


*/