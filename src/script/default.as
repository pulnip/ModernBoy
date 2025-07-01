void onCreated(Actor@ actor){
    print("Actor Created!");
}

void onInputEvent(Actor@ actor, Trigger trigger){
    auto t = actor.getTransform();
    float dt = actor.app.getDeltaTime() / 1000.0f;

    switch(trigger.button){
    case KEY_UP:
        switch(trigger.onState){
        case ButtonState::Held:
            t.position.z += 5 * dt;
            break;
        }
        break;
    case KEY_LEFT:
        switch(trigger.onState){
        case ButtonState::Held:
            t.position.x -= 5 * dt;
            break;
        }
        break;
    case KEY_DOWN:
        switch(trigger.onState){
        case ButtonState::Held:
            t.position.z -= 5 * dt;
            break;
        }
        break;
    case KEY_RIGHT:
        switch(trigger.onState){
        case ButtonState::Held:
            t.position.x += 5 * dt;
            break;
        }
        break;
    default:
        printInt(trigger.button);
    }

    actor.setTransform(t);
}

void update(Actor@ actor){

}

void onDestroyed(Actor@ actor){
    print("Actor Destroyed!");
}
