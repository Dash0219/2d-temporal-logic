#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>

#include "ASTNode.h"
#include "MaximalConsistentSet.h"

class Formula {
public:
    std::string content;
    std::unordered_set<std::string> propositions;
    std::unordered_set<std::string> temporal_formulas;
    std::unordered_set<std::string> closure_set;
    std::vector<MaximalConsistentSet> MCS;

    bool setup_formula(std::string& input);
    void clear();

    void show_tree();
    void show_propositions();
    void show_temporal_formulas();
    void show_closure_set();
    void show_MCS();

    ~Formula();
private:
    ASTNode* root = nullptr;

    bool is_binary_op(const std::string& input, int pos);
    bool parse_and_build_tree(std::string& input, ASTNode*& node);
    void generate_closure_set();
    void generate_MCS();
    void _step(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases, int current);
    void _get_mcs_from_valuations(std::vector<std::string>& props_and_temps, std::unordered_set<std::string>& true_bases);
    bool _evaluate(std::string& fmla, std::unordered_set<std::string>& true_bases, std::unordered_map<std::string, bool>& cache);
    void clear_node(ASTNode*& node);
};