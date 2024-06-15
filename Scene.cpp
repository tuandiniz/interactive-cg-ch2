//
// Created by tuan on 14/06/24.
//

#include "Scene.h"
#include <GL/glcorearb.h>
#include <glm/gtc/type_ptr.hpp>

#define GL_GLEXT_PROTOTYPES

void Scene::render() {
    for (auto child: this->getChildren()) {
        applyTransform(this->getTransform() * child->getTransform());
        child->render();
    }
}

void Scene::applyTransform(const mat4 &transform) {

}
