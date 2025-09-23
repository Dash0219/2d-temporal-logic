#pragma once

#include <iostream>
#include <list>
#include <stack>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include "ASTNode.h"
#include "Cluster.h"
#include "Trace.h"

class Formula {
public:
    using Element = std::variant<MaximalConsistentSet*, Cluster*>;

    Formula();
    Formula(std::string input);

    std::string content;
    std::unordered_set<std::string> closure_set;
    std::unordered_map<int, std::string> id_to_string;
    std::unordered_map<std::string, int> string_to_id;
    
    std::list<MaximalConsistentSet*> irreflexives;
    std::list<Cluster*> clusters;
    std::list<Element*> elements;
    
    std::unordered_map<Element*, std::vector<Element*>> pre;
    std::unordered_map<Element*, std::vector<Element*>> suc;
    std::unordered_map<void*, Element*> pointer_to_element;
    
    bool setup_formula();
    
    void show_tree();
    void show_propositions();
    void show_temporal_formulas();
    void show_closure_set();
    void show_irreflexives();
    void show_clusters(bool show_all_mcs = false);
    void show_graphs();
    
    void print_memory_usage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        
        std::cout << "Max memory used: " << usage.ru_maxrss / 1024 << " MB" << "\n";
    }
    
    private:
    ASTNode* root = nullptr;
    std::unordered_set<std::string> propositions;
    std::unordered_set<std::string> temporal_formulas;

    bool is_binary_op(const std::string& input, int pos);
    bool parse_and_build_tree(std::string& input, ASTNode*& node);

    void generate_closure_set();
    void generate_MCS_and_clusters();
    void _generate_MCS_step(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases, int current);
    void _get_mcs_from_valuations(std::unordered_set<std::string>& true_bases);
    bool _mcs_consistent(std::unordered_set<std::string>& s);

    void generate_elements();
    bool _evaluate(std::string& fmla, std::unordered_set<std::string>& true_bases, std::unordered_map<std::string, bool>& cache);
    void clear_node(ASTNode*& node);
    void generate_graphs();
    std::string transform_formula(const std::string& fmla);
};