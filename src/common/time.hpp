#ifndef TIME_HPP
#define TIME_HPP


// Return current time (since epoch) in milliseconds
inline unsigned int time_ms()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}


#endif
