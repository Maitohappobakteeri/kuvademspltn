
#include "project.hpp"
#include "print.hpp"
#include "rng.hpp"
#include "signaalidemo.hpp"

int main(int argc, char *argv[])
{
    Demo::Args args = Demo::create_args();

        println(PROJECT_NAME, " - signaali");
        seed_randgen();
        SignaaliDemo demo(args, join_command(argc, argv));
        exit(demo.run());
}
