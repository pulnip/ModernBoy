void MoveForward(AppState@ app, EntityID actor){
    Transform t = app.getTransform(actor);
    t.position.z += 1;
    app.setTransform(t, actor);
    print("MoveForward");
}
void MoveBackward(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.z -= 1;
    app.setTransform(t, actor);
    print("MoveBackward");
}
void MoveLeft(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.x -= 1;
    app.setTransform(t, actor);
    print("MoveLeft");
}
void MoveRight(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.x += 1;
    app.setTransform(t, actor);
    print("MoveRight");
}