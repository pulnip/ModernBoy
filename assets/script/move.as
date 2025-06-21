void MoveForward(Transform@ t) {
    t.moveZ(1.0f);
}
void MoveBackward(Transform@ t) {
    t.moveZ(-1.0f);
}
void MoveLeft(Transform@ t) {
    t.moveX(-1.0f);
}
void MoveRight(Transform@ t) {
    t.moveX(1.0f);
}