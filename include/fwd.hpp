#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace ModernBoy{
    using Microsoft::WRL::ComPtr;
    using Device=ID3D11Device;
    using Context=ID3D11DeviceContext;

    class Core;
    class Window;
    class RenderAdaptor;
    class ShaderAdaptor;
    class TextureAdaptor;
    class GUIAdaptor;
    class ActorManager;
    class InputSystem;

    class Actor;
    class ActorFactory;
    class Component;
    class ComponentFactory;
    class CameraComponent;
    class InputComponent;
    class LightComponent;
    class MeshComponent;
    class MoveComponent;

    class MeshObject;
    struct Light;
    struct InputState;

    struct WindowDesc;
}
