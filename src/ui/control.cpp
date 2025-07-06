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
    }
}

void RadioButton::draw(){
    size_t i=0;
    for(const auto& label: labels){
        if(!ImGui::RadioButton(label.c_str(), i++==0)){
            continue;
        }
    }
}
void RadioButton::OnEvent(Event event){
    switch(event.type){
    case EventType::StateChanged:
        index = event.index;
        break;
    default:
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
    }
}