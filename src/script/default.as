void MoveForward(AppState@ app, EntityID actor){
    Transform t = app.getTransform(actor);
    t.position.z += 1;
    app.setTransform(t, actor);
}
void MoveBackward(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.z -= 1;
    app.setTransform(t, actor);
}
void MoveLeft(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.x -= 1;
    app.setTransform(t, actor);
}
void MoveRight(AppState@ app, EntityID actor) {
    Transform t = app.getTransform(actor);
    t.position.x += 1;
    app.setTransform(t, actor);
}