//
// Created by tuan on 14/08/24.
//

#ifndef INTERACTIVE_CG_CH2_UTILS_H
#define INTERACTIVE_CG_CH2_UTILS_H

#include <string>
#include "CImg.h"

std::string readFile(const std::string& fileName);
cimg_library::CImg<unsigned char> readImage(const std::string& fileName);

#endif //INTERACTIVE_CG_CH2_UTILS_H
