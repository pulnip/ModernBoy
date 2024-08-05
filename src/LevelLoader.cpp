#include <codecvt>
#include "LevelLoader.hpp"

using namespace std;
using namespace ModernBoy;

LevelLoader::LevelLoader(const string& fileName)
:json(fileName){}

WindowDesc LevelLoader::getWindowDesc(){
    auto helper=JsonHelper(json).subset("window").value();

    auto title=helper.getString("title").value();
    auto position=helper.getPoint2i("position").value();
    auto size=helper.getPoint2i("size").value();
    auto resolution=helper.getPoint2i("resolution").value();

    return {
        // for wstring
        // {title.cbegin(), title.cend()},
        title,
        position,
        size,
        resolution
    };
}
