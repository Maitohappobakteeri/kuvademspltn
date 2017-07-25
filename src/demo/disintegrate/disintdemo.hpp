#ifndef DEMO_DISINTEGRATE_DISINTDEMO_HPP
#define DEMO_DISINTEGRATE_DISINTDEMO_HPP


#include "demo.hpp"


class DisintDemo : public Demo
{
public:

    DisintDemo(const Demo::Args& args, const std::wstring& command);
    ~DisintDemo();

protected:

    virtual bool init() override;
    virtual void cleanup() override;

    virtual bool update(float step) override;
    virtual void render() override;

private:

    void render_disint() const;

    Shader* disintShader;
    Buffer* rectTriangleStrip;
    Buffer* fullUvBuffer;

    std::shared_ptr<Texture> texture;
};

#endif
