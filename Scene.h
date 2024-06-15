//
// Created by tuan on 14/06/24.
//

#ifndef SCENE_H
#define SCENE_H
#include "Node.h"

class Scene: Node {
    const int programId;
    void applyTransform(const mat4& transform);
protected:
    ~Scene() = default;
public:
    void render() override;
};

#endif //SCENE_H
