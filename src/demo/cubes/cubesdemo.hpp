#ifndef DEMO_CUBES_CUBESDEMO_HPP
#define DEMO_CUBES_CUBESDEMO_HPP


#include "demo.hpp"

#include <btBulletDynamicsCommon.h>

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

    virtual void handle_resize(unsigned int w, unsigned int h) override;
    virtual void handle_keydown(SDL_Keycode k) override;
    virtual void handle_mouse_move(float positionX, float positionY,
                                   float moveX, float moveY) override;
    virtual void handle_mouse_down(float positionX, float positionY, int button) override;
    virtual void handle_mouse_up(float positionX, float positionY, int button) override;

private:

    void init_bullet();
    void init_ground();
    void init_cube();
    void cleanup_bullet();

    void reset_cube();
    void render_cube() const;

    Shader* colorShader;
    Buffer* cubeBuffer;
    Buffer* normalBuffer;

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    float resetCounter;

    bool isDraggingView;
    float rotationY;
    float rotationX;

    btDiscreteDynamicsWorld* dynamicsWorld;
    btAlignedObjectArray<btCollisionShape*>	collisionShapes;
	btBroadphaseInterface*	broadphase;
	btCollisionDispatcher*	dispatcher;
	btConstraintSolver*	solver;
	btDefaultCollisionConfiguration* collisionConfiguration;

    btRigidBody* cubeBody;
};

#endif
