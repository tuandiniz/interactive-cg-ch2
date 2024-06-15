//
// Created by tuan on 14/06/24.
//

#include "Node.h"
#include "glm/glm.hpp"

void Node::append(Node *child) {
    this->children.push_back(child);
}

const mat4& Node::getTransform() {
    return this->transform;
}

void Node::setTransform(const mat4 &transform) {
    this->transform = transform;
}

vector<Node *>& Node::getChildren() {
    return this->children;
}
