#include "raw_resource.hpp"
#include "util/as_typehelper.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Util;

static float getX(Transform* t){ return t->pos[0]; }
static float getY(Transform* t){ return t->pos[1]; }
static float getZ(Transform* t){ return t->pos[2]; }
static void setX(Transform* t, float x){ t->pos[0] = x; }
static void setY(Transform* t, float y){ t->pos[1] = y; }
static void setZ(Transform* t, float z){ t->pos[2] = z; }
static void moveX(Transform* t, float dx){ t->pos[0] += dx; }
static void moveY(Transform* t, float dy){ t->pos[1] += dy; }
static void moveZ(Transform* t, float dz){ t->pos[2] += dz; }
static void setPosition(Transform* t, float x, float y, float z){
    t->pos[0] = x; t->pos[1] = y; t->pos[2] = z; }
static void move(Transform* t, float dx, float dy, float dz){
    t->pos[0] += dx; t->pos[1] += dy; t->pos[2] += dz; }

int ModernBoy::Util::registerTransform(asIScriptEngine* engine){
    if(auto ret=engine->RegisterObjectType(
        "Vector3", sizeof(Vector3), asOBJ_VALUE | asOBJ_POD ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectType(
        "Vector4", sizeof(Vector4), asOBJ_VALUE | asOBJ_POD ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectType(
        "Transform", sizeof(Transform), asOBJ_REF | asOBJ_NOCOUNT ) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "float getX()",
        asFUNCTION(getX), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "float getY()",
        asFUNCTION(getY), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "float getZ()",
        asFUNCTION(getZ), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void setX(float x)",
        asFUNCTION(setX), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void setY(float y)",
        asFUNCTION(setY), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void setZ(float z)",
        asFUNCTION(setZ), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void moveX(float dx)",
        asFUNCTION(moveX), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void moveY(float dy)",
        asFUNCTION(moveY), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void moveZ(float dz)",
        asFUNCTION(moveZ), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void setPosition(float x, float y, float z)",
        asFUNCTION(setPosition), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    if(auto ret=engine->RegisterObjectMethod(
        "Transform", "void move(float dx, float dy, float dz)",
        asFUNCTION(move), asCALL_CDECL_OBJFIRST) < 0)
        return ret;
    return 0;
}