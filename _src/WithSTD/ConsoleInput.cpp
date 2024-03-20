#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include "WithSTD/ConsoleInput.hpp"

void ConsoleInput::pollEvent(){
    std::string buffer;
    std::getline(std::cin, buffer);

    std::set<char> splitter;
    std::for_each(buffer.cbegin(), buffer.cend(),
        [&splitter](const auto& c){
            splitter.emplace(c);
        }
    );

    for(auto& [event, receivers]: keyMap){
        char c=static_cast<char>(event);

        if(splitter.find(c) != splitter.end()){
            std::for_each(receivers.cbegin(), receivers.cend(),
                [&event](const auto& receiver){
                    if(not receiver.expired()){
                        receiver.lock()->onNotify(event);
                    }
                }
            );
        }
    }
}

void ConsoleInput::subscribe(const Input::Event& event, Receiver recv){
    static const auto dummy=std::vector<Receiver>();
    
    auto [it, _]=keyMap.try_emplace(event, dummy);

    it->second.emplace_back(recv);
}
