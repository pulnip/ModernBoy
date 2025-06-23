#include "component.hpp"

using namespace ModernBoy;

ResourceComponent::ResourceComponent(ResourceHandle handle)
:handle(handle){}

MeshComponent::MeshComponent(const std::vector<MeshHandle>& handles)
:meshHandles(handles){}
