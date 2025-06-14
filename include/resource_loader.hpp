#ifndef MODERNBOY_RESOURCE_LOADER_HPP
#define MODERNBOY_RESOURCE_LOADER_HPP

#include <vector>
#include "resource_manager.hpp"

namespace ModernBoy
{
    template<typename T, typename Importer, typename Dev>
    class ResourceLoader{
    public:
        using Handle = ResourceHandle<T>;
        using Handles = std::vector<Handle>;

    private:
        Importer& importer;
        ResourceManager<T>& manager;
        Dev& dev;

    public:
        ResourceLoader(Importer& importer,
            ResourceManager<T>& manager, Dev& dev)
        :importer(importer), manager(manager), dev(dev){}

        Handles load(const std::string& fileName){
            auto resources = importer.import(fileName);

            Handles handles(resources.size());
            for(size_t i=0; i<handles.size(); ++i){
                auto resource = T(resources[i], dev);
                handles[i] = manager.create(std::move(resource));
            }
            return handles;
        }
    };
}

#endif // MODERNBOY_RESOURCE_LOADER