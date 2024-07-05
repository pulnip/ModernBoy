#include <codecvt>
#include "LevelLoader.hpp"

using namespace std;
using namespace ModernBoy;

LevelLoader::LevelLoader(const string& fileName)
:json(fileName){}

WindowInfo LevelLoader::getWindowInfo(){
    auto helper=JsonHelper(json).subset("window").value();

    auto title=helper.getString("title").value();
    auto position=helper.getVec2i("position").value();
    auto size=helper.getVec2i("size").value();

    return {
        {title.cbegin(), title.cend()},
        position,
        size
    };
}
