void onCreated(Entity@ entity){
    print("Actor Created!");
}

void update(Entity@ entity, float dt){
    Rigidbody@ rc = cast<Rigidbody>(
        entity.GetComponentByClassName("Rigidbody"));
    if(rc is null)
        print("MoveComponent: Cannot find Rigidbody Component");

    float vx = 0, vz = 0;

    if(input.query(KEY_LEFT, Held))
        vx -= 5;
    if(input.query(KEY_RIGHT, Held))
        vx += 5;

    if(input.query(KEY_UP, Held))
        vz += 5;
    if(input.query(KEY_DOWN, Held))
        vz -= 5;

    rc.velocity.x = vx;
    rc.velocity.z = vz;
}

void onDestroyed(Entity@ entity){
    print("Actor Destroyed!");
}
