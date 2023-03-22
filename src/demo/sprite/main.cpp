
#include "project.hpp"
#include "print.hpp"
#include "spritedemo.hpp"


const char* DEFAULT_SPRITE_FILENAME = "res/sprite/arrow/sprite.txt";


int main(int argc, char *argv[])
{
    Demo::Args args = Demo::create_args();

    // Handle basic options
    if(args.disablePrint)
    {
        disable_print();
    }
    if(args.printHelp)
    {
    }

    if(args.shouldRun)
    {
        std::string spritePath;
            println("using sprite file: ", DEFAULT_SPRITE_FILENAME);
            spritePath = DEFAULT_SPRITE_FILENAME;

        println(PROJECT_NAME, " - sprite");
        SpriteDemo demo(spritePath, args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
