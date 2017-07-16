
#include "project.hpp"
#include "print.hpp"
#include "spritedemo.hpp"


int main(int argc, char *argv[])
{
    println(PROJECT_NAME, " - sprite");

    if(argc < 2)
    {
        println("Give a path for the sprite!");
        exit(1);
    }
    else if(argc > 2)
    {
        println("Give only one sprite path!");
    }
    else
    {
        SpriteDemo demo(argv[1]);
        exit(demo.run());
    }
}
