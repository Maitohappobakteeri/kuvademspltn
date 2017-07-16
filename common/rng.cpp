#include "rng.hpp"


#include <chrono>


std::mt19937_64 randGen;


void seed_randgen()
{
    using namespace std::chrono;
    randGen.seed(system_clock::now().time_since_epoch().count());
}
