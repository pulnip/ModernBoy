#include "log.hpp"
#include "app_state.hpp"
#include "game/context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Context::Context(AppState& app)
:app(app), archetypeMap(), actorTable(),
draw(*this), action(*this, app.scriptInvoker),
physics(*this), lifespan(*this){}


void Context::update(DeltaTime dt){
    auto drawFut = draw.update(dt);
    while(!drawFut.done())
        drawFut.next();
}

uint32_t Context::issueID(){
    return app.issueID();
}

DeltaTime Context::getDeltaTime(){
    return app.getDeltaTime();
}

static void assignEntityID(SparseChunk& components,
    EntityID actor);

EntityID Context::create(
    ArchetypeBit bit, SparseChunk&& chunk
){
    EntityID actor_id = issueID();
    GameTrace("  Actor ID issued: {}, archetype: {}", actor_id, bit);
    assignEntityID(chunk, actor_id);

    auto chunkIndex = archetypeMap.insert(
        bit, chunk);
    auto [it, ret] = actorTable.emplace(
        actor_id, ComponentInfo{bit, chunkIndex});
    if(!ret){
        std::string actorInfo = std::format(
            "Actor{} type:{} Not Created!",
            actor_id, bit
        );
        throw std::runtime_error(actorInfo);
    }
    return actor_id;
}
static void assignEntityID(SparseChunk& chunk,
    EntityID actor
){
    chunk.transform.actor = actor;
    chunk.camera.actor = actor;
    chunk.mesh.actor = actor;
    chunk.input.actor = actor;
}

void Context::destroy(EntityID actor){
    auto info = actorTable.at(actor);
    archetypeMap.at(info.bit).free(info.chunkIndex);
    actorTable.erase(actor);
}

template<>
const std::vector<ViewTask>& Context::getBuffer() const{
    return draw.viewTasks;
}
template<>
const std::vector<DrawTask>& Context::getBuffer() const{
    return draw.drawTasks;
}

DeltaTime Context::getDeltaTime() const{
    return app.getDeltaTime();
}

