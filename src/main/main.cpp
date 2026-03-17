#include "Abstract/Player/Player.h"
#include <iostream>
int main(){
    Player test = Player("Test");

    std::cout << "Name: " << test.getName() << "\n";
}