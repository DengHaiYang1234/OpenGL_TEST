//
//  Texture.hpp
//  1.你好，窗口
//
//  Created by 邓海洋 on 2022/10/1.
//

#ifndef Texture_hpp
#define Texture_hpp

#include <glad/glad.h>
#include <stdio.h>
#include <string>
#include <string_view>
#include <iostream>
#include <vector>

using namespace std;

class TextureUtilities
{
public:
    unsigned int id;
    string type;
    string path;
    unsigned int LoadTextureFromDirectory(const char *path, const string &directory, bool gamma = false);
    unsigned int LoadTextureFromPath(string path);
    unsigned int LoadCubeMap(vector<string> faces);
    void SetFlipVertically(bool isFlipVert);
};

#endif /* Texture_hpp */
