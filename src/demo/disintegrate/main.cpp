
#include "project.hpp"
#include "print.hpp"
#include "rng.hpp"
#include "disintdemo.hpp"

int main(int argc, char *argv[])
{
    // create Demo::Args from options
    Demo::Args args = Demo::create_args();

    // Handle basic options
    if(args.disablePrint)
    {
        disable_print();
    }

    if(args.shouldRun)
    {
        println(PROJECT_NAME, " - disintegrate");
        seed_randgen();
        DisintDemo demo(args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
