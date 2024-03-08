#include <iostream>

void push(int* stack, int& ptr){
    int new_num;
    std::cin>>new_num;

    stack[ptr++]=new_num;
}

int pop(int* stack, int& ptr){
    if(ptr == -1) return -1;
    return stack[ptr--];
}

int len(int* stack, int& ptr){
    return ptr+1;
}

bool isEmpty(int* stack, int& ptr){
    return ptr==-1;
}

int top(int* stack, int& ptr){
    return stack[ptr];
}

int main(void){
    int stack[1000000];
    int pointer=-1;

    int cmd_num;
    std::cin >> cmd_num;

    for(int i=0; i<cmd_num; ++i){
        int command;
        std::cin>>command;

        switch(command){
        case 1:
            push(stack, pointer);
            break;
        case 2:
            std::cout<<pop(stack, pointer)<<'\n';
            break;
        case 3:
            std::cout<<len(stack, pointer)<<'\n';
            break;
        case 4:
            std::cout<<static_cast<int>(isEmpty(stack, pointer))<<'\n';
            break;
        case 5:
            std::cout<<top(stack, pointer)<<'\n';
        }
    }
}