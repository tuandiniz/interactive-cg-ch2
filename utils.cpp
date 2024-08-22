//
// Created by tuan on 14/08/24.
//

#include <string>
#include <fstream>

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
