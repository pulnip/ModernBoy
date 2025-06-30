void onCreated(Actor@ actor){
    print("Actor Created!");
}

void onInputEvent(Actor@ actor, Trigger trigger){
    auto t = actor.getTransform();

    switch(trigger.button){
    case KEY_UP:
        switch(trigger.onState){
        case ButtonState::Pressed:
            t.position.z += 1;
            break;
        }
        break;
    case KEY_LEFT:
        switch(trigger.onState){
        case ButtonState::Pressed:
            t.position.x -= 1;
            break;
        }
        break;
    case KEY_DOWN:
        switch(trigger.onState){
        case ButtonState::Pressed:
            t.position.z -= 1;
            break;
        }
        break;
    case KEY_RIGHT:
        switch(trigger.onState){
        case ButtonState::Pressed:
            t.position.x += 1;
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
