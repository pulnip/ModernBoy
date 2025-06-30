#include <print>
#include <angelscript.h>
#include "common/type.hpp"
#include "raw_resource.hpp"
#include "script/as_typehelper.hpp"
#include "app_state.hpp"
#include "input/state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

int ModernBoy::Script::registerTransform(asIScriptEngine* engine){
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
    // register Transform
    if(auto ret=engine->RegisterObjectType(
        "Transform", sizeof(Transform),
        asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform",
        "Vec3 position", asOFFSET(Transform, position)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform",
        "Vec4 rotation", asOFFSET(Transform, rotation)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform",
        "Vec3 scale", asOFFSET(Transform, scale)) < 0)
        return ret;
    return 0;
}

int ModernBoy::Script::registerKeyevent(asIScriptEngine* engine){
    using namespace ModernBoy::Input;
    // register ButtonState
    if(auto ret=engine->RegisterEnum("ButtonState") < 0)
        return ret;
    for(size_t i=0; i<STATE_INVALID; ++i){
        if(auto ret=engine->RegisterEnumValue("ButtonState",
            toText(static_cast<ButtonState>(i)).c_str(), i))
            return ret;
    }
    // register Button
    if(auto ret=engine->RegisterEnum("Button") < 0)
        return ret;
    for(size_t i=0; i<KEY_UNKNOWN; ++i){
        auto text = std::format("KEY_{}", toText(static_cast<Button>(i)));
        if(auto ret=engine->RegisterEnumValue("Button",
            text.c_str(), i))
            return ret;
    }
    // register Trigger
    if(auto ret=engine->RegisterObjectType(
        "Trigger", sizeof(Trigger),
        asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Trigger",
        "Button button", asOFFSET(Trigger, button)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Trigger",
        "ButtonState onState", asOFFSET(Trigger, onState)) < 0)
        return ret;
    return 0;
}

static Transform getTransform(Game::Actor* actor){
    auto it = actor->app.actorTable.find(actor->id);
    if(it == actor->app.actorTable.end()){
        std::println("Actor No.{} not exists.", actor->id);
        return identity();
    }
    auto [_, info] = *it;
    return actor->app.archetypeMap.getTransformComponent(
        info.bit, info.chunkIndex).value;
}

static void setTransform(Game::Actor* actor, Transform transform){
    auto it = actor->app.actorTable.find(actor->id);
    if(it == actor->app.actorTable.end()){
        std::println("Actor No.{} not exists.", actor->id);
        return;
    }
    auto [_, info] = *it;
    auto component = TransformComponent{
        .actor = actor->id,
        .isActive = true,
        .value = transform
    };
    component.value = transform;
    actor->app.archetypeMap.setTransformComponent(
        component, info.bit, info.chunkIndex);
}

int ModernBoy::Script::registerActor(asIScriptEngine* engine){
    if(auto ret=engine->RegisterObjectType(
        "Actor", sizeof(Game::Actor),
        asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Actor", "Transform getTransform()",
        asFUNCTION(getTransform), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Actor", "void setTransform(Transform)",
        asFUNCTION(setTransform), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    return 0;
}