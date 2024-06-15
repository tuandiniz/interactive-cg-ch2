//
// Created by tuan on 14/06/24.
//

#ifndef NODE_H
#define NODE_H

#include <vector>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Node {
protected:
    ~Node() = default;
private:
    vector<Node*> children;
    mat4 transform = mat4(1.0);
public:
    const mat4& getTransform();
    void setTransform(const mat4& transform);
    virtual void render() = 0;
    void append(Node* child);
    vector<Node*>& getChildren();
};

#endif //NODE_H
