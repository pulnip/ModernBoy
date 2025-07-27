#include <imgui.h>
#include "ui/control.hpp"

using namespace ModernBoy::UI;

void CheckBox::draw(){
    if(!ImGui::Checkbox(label.c_str(), &checked)){
        return;
    }
}
void CheckBox::OnEvent(Event event){
    switch(event.type){
    case EventType::StateChanged:
        checked = event.checked;
        break;
    default:
        break;
    }
}

void RadioButton::draw(){
    for(Index i=0; i<labels.size(); ++i){
        if(ImGui::RadioButton(labels[i].c_str(), index==i))
            index = i;
        if(i != labels.size()-1)
            ImGui::SameLine();
    }
}
void RadioButton::OnEvent(Event event){
    switch(event.type){
    case EventType::StateChanged:
        index = event.index;
        break;
    default:
        break;
    }
}

void Slider::draw(){
    if(!ImGui::SliderFloat(label.c_str(),
        &value, minValue, maxValue)){
        return;
    }

}
void Slider::OnEvent(Event event){
    switch(event.type){
    case EventType::StateChanged:
        value = event.value;
        break;
    default:
        break;
    }
}

void SameLine::draw(){
    ImGui::SameLine();
}
void SameLine::OnEvent(Event){}
