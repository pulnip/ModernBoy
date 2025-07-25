#include "log.hpp"
#include "app_state.hpp"
#include "game/context.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Game;

Context::Context(AppState& app)
:app(app),
debug(registry), draw(registry),
action(registry, app.scriptInvoker),
physics(registry), lifespan(registry){}


void Context::update(DeltaTime dt){
    auto actionFut = action.update(dt);
    while(!actionFut.done())
        actionFut.next();

    auto physicsFut = physics.update(dt);
    while(!physicsFut.done())
        physicsFut.next();

    auto debugFut = debug.update(dt);
    while(!debugFut.done())
        debugFut.next();

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

static void assignEntityID(SparseChunk& chunk,
    EntityID actor
){
    chunk.transform.actor = actor;
    chunk.camera.actor = actor;
    chunk.mesh.actor = actor;
    chunk.action.actor = actor;
    chunk.input.actor = actor;
    chunk.rigidbody.actor = actor;
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

