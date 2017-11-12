#include "texture.hpp"
#include <fstream>
#include <sstream>
#include <string>

Texture readTexture(const std::string& filepath)
{
    using namespace std;
	stringstream ss;
	ifstream f(filepath);
	string temp;
    getline(f, temp);
    getline(f, temp);
	size_t width, height, color;
	f >> width >> height >> color;
    auto texture = Texture(width, height);
    for (size_t i = 0; i < texture.size(); ++i)
    {
        f >> texture[i](RED) >> texture[i](GREEN) >> texture[i](BLUE);
        texture[i](DUMMY) = 0.0;
	}
    return texture;
}
