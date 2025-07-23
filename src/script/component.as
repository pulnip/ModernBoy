class IComponent{
    void Start(){}
    void Update(Entity@, float){}
    Component@ GetComponentByClassName(Entity@ entity, const string& in name){
        return entity.GetComponentByClassNameImpl(name);
    }
}