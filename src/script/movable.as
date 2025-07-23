class Movable: IComponent{
    void Start(){
        print("Movable Start");
    }
    void Update(Entity@, float){
        print("Movable Update");
    }
}