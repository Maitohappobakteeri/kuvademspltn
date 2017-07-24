
#include "project.hpp"
#include "print.hpp"
#include "rng.hpp"
#include "cubesdemo.hpp"

#include <boost/program_options.hpp>


int main(int argc, char *argv[])
{
    namespace opt = boost::program_options;

    // create options
    opt::options_description desc(Demo::options());

    // parse options
    opt::variables_map vm;
    opt::store(opt::command_line_parser(argc, argv).options(desc).extra_parser(is_wid_option)
               .run(), vm);
    opt::notify(vm);

    // create Demo::Args from options
    Demo::Args args = Demo::create_args(vm);

    // Handle basic options
    if(args.disablePrint)
    {
        disable_print();
    }
    if(args.printHelp)
    {
        println(desc);
    }

    if(args.shouldRun)
    {
        println(PROJECT_NAME, " - cubes");
        seed_randgen();
        CubesDemo demo(args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
