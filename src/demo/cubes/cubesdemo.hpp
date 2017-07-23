#ifndef DEMO_CUBES_CUBESDEMO_HPP
#define DEMO_CUBES_CUBESDEMO_HPP


#include "demo.hpp"

#include <string>


class CubesDemo : public Demo
{
public:

    CubesDemo(const Demo::Args& args, const std::wstring& command);
    ~CubesDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;
};

#endif
