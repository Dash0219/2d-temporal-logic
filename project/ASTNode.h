#pragma once

#include <string>

class ASTNode {
public:
    std::string content;
    ASTNode* left;
    ASTNode* right;

    ASTNode();
    ASTNode(const std::string& c);
};
