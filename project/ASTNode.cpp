#include "ASTNode.h"

ASTNode::ASTNode() : content(""), left(nullptr), right(nullptr) {}

ASTNode::ASTNode(const std::string& c) : content(c), left(nullptr), right(nullptr) {}
