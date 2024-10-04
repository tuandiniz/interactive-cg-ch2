//
// Created by tuan on 14/08/24.
//

#include <string>
#include <fstream>
#include <iostream>

#include <cstdio>
#include <jpeglib.h>
#include <jerror.h>

// Define plugin and include the CImg Library.
#define cimg_plugin "plugins/jpeg_buffer.h"
#include "CImg.h"
using namespace cimg_library;
using namespace std;

string readFile(const string& fileName) {
    ifstream file(fileName);
    string line;
    string shaderSource;
    if(file.is_open()) {
        while(getline(file, line)) {
            shaderSource.append(line);
            shaderSource.append("\n");
        }
    }

    return shaderSource;
}

CImg<unsigned char> readImage(const std::string& fileName) {
    
    const char *filename_input = fileName.c_str();
    std::FILE *file_input = std::fopen(filename_input,"rb");
    if (!file_input) { std::fprintf(stderr,"Input JPEG file not found !"); std::exit(0); }

    unsigned buf_size = 500000; // Put the file size here !
    JOCTET *buffer_input = new JOCTET[buf_size];
    if (std::fread(buffer_input,sizeof(JOCTET),buf_size,file_input)) std::fclose(file_input);

    CImg<unsigned char> img;
    img.load_jpeg_buffer(buffer_input, buf_size);
    delete[] buffer_input;

    img.permute_axes("cxyz");
    return img;
}
