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
    Formula(std::string test);

    std::string content;
    std::unordered_set<std::string> propositions;
    std::unordered_set<std::string> temporal_formulas;
    std::unordered_set<std::string> closure_set;
    // std::vector<MaximalConsistentSet> MCS;
    // std::vector<MaximalConsistentSet> irreflexives;
    // std::vector<Cluster> clusters;
    std::list<MaximalConsistentSet> mcs_storage;
    std::list<Cluster> cluster_storage;
    std::list<Element> element_storage;

    std::list<MaximalConsistentSet*> irreflexives;
    std::list<Cluster*> clusters;
    std::list<Element*> elements;

    std::unordered_map<Element, std::vector<Element>> pre;
    std::unordered_map<Element, std::vector<Element>> suc;

    bool setup_formula();

    void show_tree();
    void show_propositions();
    void show_temporal_formulas();
    void show_closure_set();
    void show_MCS();
    void show_irreflexives();
    void show_clusters(bool show_all_mcs = false);
    void show_graphs();

private:
    ASTNode* root = nullptr;

    bool is_binary_op(const std::string& input, int pos);
    bool parse_and_build_tree(std::string& input, ASTNode*& node);
    void generate_closure_set();
    void generate_MCS_and_clusters();
    void _generate_MCS_step(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases, int current);
    void _get_mcs_from_valuations(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases);
    void generate_elements();
    bool _evaluate(std::string& fmla, std::unordered_set<std::string>& true_bases, std::unordered_map<std::string, bool>& cache);
    void clear_node(ASTNode*& node);
    void generate_graphs();
};