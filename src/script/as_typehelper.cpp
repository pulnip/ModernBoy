#include <print>
#include <angelscript.h>
#include "common/type.hpp"
#include "raw_resource.hpp"
#include "script/as_typehelper.hpp"
#include "app_state.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

int ModernBoy::Script::registerTransform(asIScriptEngine* engine){
    // register Vec3
    if(auto ret=engine->RegisterObjectType(
        "Vec3", sizeof(Vec3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3", "float x",
        asOFFSET(Vec3, x)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3", "float y",
        asOFFSET(Vec3, y)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec3", "float z",
        asOFFSET(Vec3, z)) < 0)
        return ret;
    // register Vec4
    if(auto ret=engine->RegisterObjectType(
        "Vec4", sizeof(Vec4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4", "float x",
        asOFFSET(Vec4, x)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4", "float y",
        asOFFSET(Vec4, y)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4", "float z",
        asOFFSET(Vec4, z)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Vec4", "float w",
        asOFFSET(Vec4, w)) < 0)
        return ret;
    // register Transform
    if(auto ret=engine->RegisterObjectType(
        "Transform", sizeof(Transform), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDAK) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform", "Vec3 position",
        asOFFSET(Transform, position)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform", "Vec4 rotation",
        asOFFSET(Transform, rotation)) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectProperty("Transform", "Vec3 scale",
        asOFFSET(Transform, scale)) < 0)
        return ret;
    return 0;
}

static Transform getTransform(AppState* appState, EntityID id){
    auto it = appState->actorTable.find(id);
    if(it == appState->actorTable.end()){
        std::println("Actor No.{} not exists.", id);
        return identity();
    }
    auto [_, info] = *it;
    return appState->archetypeMap.getTransformComponent(
        info.bit, info.chunkIndex).value;
}
static void setTransform(AppState* appState, Transform transform, EntityID id){
    auto it = appState->actorTable.find(id);
    if(it == appState->actorTable.end()){
        std::println("Actor No.{} not exists.", id);
        return;
    }
    auto [_, info] = *it;
    auto component = appState->archetypeMap.
        getTransformComponent(info.bit, info.chunkIndex);
    if(component.actor != id){
        std::println("Actor Mismatched. Expected: {}, but Got {}",
            id, component.actor);
        return;
    }
    component.value = transform;
    appState->archetypeMap.setTransformComponent(
        component, info.bit, info.chunkIndex);
}

int ModernBoy::Script::registerAppState(asIScriptEngine* engine){
    if(auto ret=engine->RegisterObjectType(
        "AppState", sizeof(AppState), asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectType(
        "EntityTable", sizeof(EntityTable), asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectType(
        "ArchetypeMap", sizeof(ArchetypeMap), asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    return 0;
}

int ModernBoy::Script::registerActor(asIScriptEngine* engine){
    if(auto ret=engine->RegisterTypedef("EntityID", "uint") < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "AppState", "Transform getTransform(EntityID)",
        asFUNCTION(getTransform), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "AppState", "void setTransform(Transform, EntityID)",
        asFUNCTION(setTransform), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    return 0;
}