
#include "project.hpp"
#include "print.hpp"
#include "rng.hpp"
#include "roiskedemo.hpp"

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
        println(PROJECT_NAME, " - roiske");
        seed_randgen();
        RoiskeDemo demo(args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
