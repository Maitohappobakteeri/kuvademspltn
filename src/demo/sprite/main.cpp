
#include "project.hpp"
#include "print.hpp"
#include "spritedemo.hpp"

#include <boost/program_options.hpp>


const char* DEFAULT_SPRITE_FILENAME = "res/sprite/arrow/sprite.txt";


int main(int argc, char *argv[])
{
    namespace opt = boost::program_options;

    // create options
    opt::options_description desc(Demo::options());
    desc.add_options()
        ("spritefile", opt::value<std::string>(), "sprite to be shown")
    ;

    // create positional
    opt::positional_options_description pt;
    pt.add("spritefile", 1);

    // parse options
    opt::variables_map vm;
    opt::store(opt::command_line_parser(argc, argv).options(desc).positional(pt)
               .extra_parser(is_wid_option).run(), vm);
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
        std::string spritePath;
        if(vm.count("spritefile") == 0)
        {
            println("using sprite file: ", DEFAULT_SPRITE_FILENAME);
            spritePath = DEFAULT_SPRITE_FILENAME;
        }
        else
        {
            spritePath = vm["spritefile"].as<std::string>();
        }

        println(PROJECT_NAME, " - sprite");
        SpriteDemo demo(spritePath, args, join_command(argc, argv));
        exit(demo.run());
    }
    else
    {
        exit(0);
    }
}
