void onCreated(Entity@ entity){
    print("Actor Created!");
}

void update(Entity@ entity){
    // auto t = actor.getTransform();
    // float dt = actor.world.getDeltaTime() / 1000.0f;

    // if(input.query(KEY_UP, Held))
    //     t.position.z += 5 * dt;
    // if(input.query(KEY_DOWN, Held))
    //     t.position.z -= 5 * dt;
    // if(input.query(KEY_LEFT, Held))
    //     t.position.x -= 5 * dt;
    // if(input.query(KEY_RIGHT, Held))
    //     t.position.x += 5 * dt;

    // actor.setTransform(t);
}

void onDestroyed(Entity@ entity){
    print("Actor Destroyed!");
}
