#include <print>
#include <angelscript.h>
#include "core/math/type.hpp"
#include "engine/raw_resource.hpp"
#include "engine/script/as_typehelper.hpp"
#include "engine/script/invoker.hpp"
#include "engine/input/device.hpp"
#include "engine/input/state.hpp"
#include "../../../src/game/context.hpp"
#include "../../../src/game/component.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;
using namespace ModernBoy::Game;

TypeHelper::TypeHelper(asIScriptEngine* engine,
    Input::Device& device)
:engine(engine), device(device){}

int TypeHelper::registerAll(){
    if(auto ret = registerBasicTypes())
        return ret;
    if(auto ret = registerKeyevent())
        return ret;
    if(auto ret = registerActor())
        return ret;
    if(auto ret = registerComponent())
        return ret;
    if(auto ret = registerGlobalProperty())
        return ret;
    GameDebug(" Successfully bind C++ Types to Angelscript.");
    return 0;
}

int TypeHelper::registerGlobalProperty(){
    if(auto ret=engine->RegisterObjectType(
        "Input", sizeof(Input::Device),
        asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    if(auto ret=engine->RegisterGlobalProperty(
        "Input input", &device) < 0)
        return ret;
    // if(auto ret=engine->RegisterObjectMethod(
    //     "Input", "bool query(KeyCode keyCode, KeyState keyState)",
    //     asMETHOD(Input::Device, query), asCALL_THISCALL) < 0)
    //     return ret;
    
    return 0;
}

int TypeHelper::registerBasicTypes(){
    // register Vec3
    if(auto ret=engine->RegisterObjectType(
        "Vec3", sizeof(Vec3),
        asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3",
        "float x", asOFFSET(Vec3, x)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3",
        "float y", asOFFSET(Vec3, y)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3",
        "float z", asOFFSET(Vec3, z)) < 0)
        return ret;
    // register Vec4
    if(auto ret=engine->RegisterObjectType(
        "Vec4", sizeof(Vec4),
        asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4",
        "float x", asOFFSET(Vec4, x)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4",
        "float y", asOFFSET(Vec4, y)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4",
        "float z", asOFFSET(Vec4, z)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4",
        "float w", asOFFSET(Vec4, w)) < 0)
        return ret;

    return 0;
}

int TypeHelper::registerKeyevent(){
    using namespace ModernBoy::Input;
    // register KeyState
    if(auto ret=engine->RegisterEnum("KeyState") < 0)
        return ret;
    for(size_t i=0; i<STATE_INVALID; ++i){
        if(auto ret=engine->RegisterEnumValue("KeyState",
            toText(static_cast<KeyState>(i)).c_str(), i))
            return ret;
    }
    // register Key
    if(auto ret=engine->RegisterEnum("KeyCode") < 0)
        return ret;
    for(size_t i=0; i<KEY_UNKNOWN; ++i){
        auto text = std::format("KEY_{}", toText(static_cast<KeyCode>(i)));
        if(auto ret=engine->RegisterEnumValue("KeyCode",
            text.c_str(), i))
            return ret;
    }
    return 0;
}

int TypeHelper::registerActor(){
    int typeId = engine->GetTypeIdByDecl("uint64");
    if (typeId < 0)
        std::println("Angelscript Not Support uint64.");
    else
        std::println("Angelscript Supprty uint64, typeId = {}", typeId);
    if(auto ret=engine->RegisterObjectType(
        "Entity", sizeof(Entity),
        asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;

    return 0;
}



template<typename T>
static T* findComponent(Entity* entity){
    if(entity == nullptr)
        return nullptr;
    if(!subset(bit_of<T>(), entity->bit))
        return nullptr;

    return static_cast<T*>(Util::add(
        entity->chunk, offset_of<T>(entity->bit)
    ));
}

static void* GetComponentByClassName(Entity* entity, const std::string& name){
    if(entity == nullptr)
        return nullptr;

    if(name == "Transform")
        return findComponent<Transform>(entity);
    if(name == "Rigidbody")
        return findComponent<Rigidbody>(entity);
    return nullptr;
    // other components...
}

template<typename T>
static T* componentCast(void* component){
    return static_cast<T*>(component);
}

int TypeHelper::registerComponent(){
    if(auto ret=engine->RegisterObjectType(
        "Transform",
        sizeof(Transform), asOBJ_REF | asOBJ_NOCOUNT) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty(
        "Transform", "Vec3 position",
        asOFFSET(Transform, position)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty(
        "Transform", "Vec4 rotation",
        asOFFSET(Transform, rotation)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty(
        "Transform", "Vec3 scale",
        asOFFSET(Transform, scale)) < 0)
        return ret;

    if(auto ret=engine->RegisterObjectType(
        "Rigidbody", sizeof(Rigidbody),
        asOBJ_REF | asOBJ_NOCOUNT) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty(
        "Rigidbody", "Vec3 velocity",
        asOFFSET(Rigidbody, velocity)) < 0)
        return ret;

    if(auto ret=engine->RegisterObjectType(
        "Component", sizeof(void*),
        asOBJ_REF | asOBJ_NOCOUNT) < 0)
        return ret;
    if(auto ret = engine->RegisterObjectMethod(
        "Component", "Transform@ opCast()",
        asFUNCTION(componentCast<Transform>), asCALL_CDECL_OBJLAST) < 0)
        return ret;
    if(auto ret = engine->RegisterObjectMethod(
        "Component", "Rigidbody@ opCast()",
        asFUNCTION(componentCast<Rigidbody>), asCALL_CDECL_OBJLAST) < 0)
        return ret;

    if(auto ret=engine->RegisterObjectMethod(
        "Entity", "Component@ GetComponentByClassNameImpl(const string& in)",
        asFUNCTION(GetComponentByClassName), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    return 0;
}
