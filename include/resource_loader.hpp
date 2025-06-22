#ifndef MODERNBOY_RESOURCE_LOADER_HPP
#define MODERNBOY_RESOURCE_LOADER_HPP

#include <vector>
#include "fwd.hpp"

namespace ModernBoy
{
    template<typename RawResource, typename Resource>
    class ResourceLoader{
    public:
        using Handle = ResourceHandle<Resource>;
        using Handles = std::vector<Handle>;

    private:
        AppState& app;

    public:
        ResourceLoader(AppState& app):app(app){}

        Handles load(const std::string& fileName){
            auto resources = ModernBoy::import<RawResource>(app, fileName);

            Handles handles(resources.size());
            for(size_t i=0; i<handles.size(); ++i){
                auto resource = Resource(resources[i], app);
                handles[i] = manage<Resource>(app, std::move(resource));
            }
            return handles;
        }
    };
}

#endif // MODERNBOY_RESOURCE_LOADER_HPP