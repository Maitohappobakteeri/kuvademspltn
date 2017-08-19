#include "cubesdemo.hpp"


#include "print.hpp"
#include "rng.hpp"

#include <glm/gtc/matrix_transform.hpp>


CubesDemo::CubesDemo(const Demo::Args& args, const std::wstring& command)
    :Demo(args, command)
{
    // renderDemoInfo = false;
}


CubesDemo::~CubesDemo()
{

}


bool CubesDemo::init()
{
    if(!Demo::init()) return false;

    colorShader = new Shader("res/cubes/shader/color.fragmentshader", GL_FRAGMENT_SHADER,
                             "res/cubes/shader/color.vertexshader", GL_VERTEX_SHADER);

    const GLfloat cubepoints[] =
    {
        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        1.0,-1.0,-1.0,
        1.0,-1.0,1.0,

        -1.0,1.0,-1.0,
        -1.0,1.0,1.0,
        1.0,1.0,-1.0,
        -1.0,1.0,1.0,
        1.0,1.0,-1.0,
        1.0,1.0,1.0,

        -1.0,-1.0,-1.0,
        1.0,-1.0,-1.0,
        1.0,1.0,-1.0,
        -1.0,-1.0,-1.0,
        1.0,1.0,-1.0,
        -1.0,1.0,-1.0,

        -1.0,-1.0,1.0,
        1.0,-1.0,1.0,
        1.0,1.0,1.0,
        -1.0,-1.0,1.0,
        1.0,1.0,1.0,
        -1.0,1.0,1.0,

        -1.0,-1.0,-1.0,
        -1.0,-1.0,1.0,
        -1.0,1.0,-1.0,
        -1.0,-1.0,1.0,
        -1.0,1.0,-1.0,
        -1.0,1.0,1.0,

        1.0,-1.0,-1.0,
        1.0,-1.0,1.0,
        1.0,1.0,-1.0,
        1.0,-1.0,1.0,
        1.0,1.0,-1.0,
        1.0,1.0,1.0,
    };

    const GLfloat normalpoints[] =
    {
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,
        0.0, -1.0, 0.0,

        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,
        0.0, 1.0, 0.0,

        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,
        0.0, 0.0, -1.0,

        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0,

        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,
        -1.0, 0.0, 0.0,

        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 0.0, 0.0
    };

    cubeBuffer = new Buffer();
    cubeBuffer->buffer_data(GL_ARRAY_BUFFER, cubepoints,
                            sizeof(cubepoints), GL_STATIC_DRAW);

    normalBuffer = new Buffer();
    normalBuffer->buffer_data(GL_ARRAY_BUFFER, normalpoints,
                              sizeof(normalpoints), GL_STATIC_DRAW);

    viewMatrix = glm::lookAt(glm::vec3{-0.5f,4.0f,-5.0f}*1.5f, glm::vec3{0.0f,0.0f,0.0f},
                             glm::vec3{0.0f,1.0f,0.0f});

    resetCounter = 0;
    isDraggingView = false;
    rotationY = 0;
    rotationX = 0;

	init_bullet();

    return true;
}


void CubesDemo::cleanup()
{
    cleanup_bullet();

    delete colorShader;
    delete cubeBuffer;
    delete normalBuffer;

    Demo::cleanup();
}


bool CubesDemo::update(float step)
{
    resetCounter += step;
    if(resetCounter >= 3.0)
    {
        resetCounter = 0;
        reset_cube();
    }

	dynamicsWorld->stepSimulation(step);

    glm::mat4 viewRotationMat = glm::rotate(glm::rotate(glm::mat4(), rotationY, {0,1,0}),
                                            rotationX, {1,0,0});
    glm::vec4 rotatedEyePosition = viewRotationMat * (glm::vec4{-0.5f, 4.0f, -5.0f, 0.0f} * 1.5f);
    viewMatrix = glm::lookAt(glm::vec3(rotatedEyePosition),
                             glm::vec3{0.0f,0.0f,0.0f},
                             glm::vec3{0.0f,1.0f,0.0f});

    return false;
}


void CubesDemo::render()
{
    renderer->clear();
    // renderer->render_rectangle({0.01,0.025,0.1}, {0,0}, renderer->get_view_scale(), 0);
    render_cube();
}


void CubesDemo::handle_resize(unsigned int w, unsigned int h)
{
    Demo::handle_resize(w, h);

    const glm::vec2 viewScale = renderer->get_view_scale();
    projectionMatrix = glm::perspective(glm::radians(90.0f), viewScale.x/viewScale.y,
                                        0.1f, 100.0f);
}


void CubesDemo::handle_keydown(SDL_Keycode k)
{
    switch(k)
    {
    case SDLK_SPACE:
        resetCounter = 0;
        reset_cube();
        break;
    }
}


void CubesDemo::handle_mouse_move(float positionX, float positionY, float moveX, float moveY)
{
    if(isDraggingView)
    {
        rotationY -= moveX * 2.5f;
        rotationX += moveY * 2.5f;
    }
}


void CubesDemo::handle_mouse_down(float positionX, float positionY, int button)
{
    if(button == 1)
    {
        isDraggingView = true;
    }
}


void CubesDemo::handle_mouse_up(float positionX, float positionY, int button)
{
    if(button == 1)
    {
        isDraggingView = false;
    }
}


void CubesDemo::init_bullet()
{
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    broadphase = new btDbvtBroadphase();
    solver = new btSequentialImpulseConstraintSolver;

    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,
                                                broadphase,solver,
                                                collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0,-10,0));

    init_ground();
    init_cube();

    reset_cube();
}


void CubesDemo::init_ground()
{
    btBoxShape* groundShape = new btBoxShape(btVector3(btScalar(50),btScalar(50),btScalar(50)));
    collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0,-50,0));
    btVector3 localInertia(0,0,0);
    btDefaultMotionState* motionState = new btDefaultMotionState(groundTransform);

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
}


void CubesDemo::init_cube()
{
    btBoxShape* shape = new btBoxShape(btVector3(1,1,1));
    collisionShapes.push_back(shape);

    btTransform trans;
    trans.setIdentity();

    btVector3 localInertia(0,0,0);
    shape->calculateLocalInertia(1.0f, localInertia);

    btDefaultMotionState* motionState = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motionState, shape, localInertia);
    cubeBody = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(cubeBody);
}


void CubesDemo::cleanup_bullet()
{
    while(dynamicsWorld->getNumCollisionObjects() != 0)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[0];
        btRigidBody* body = btRigidBody::upcast(obj);
        if(body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

	for(int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		delete shape;
	}
	collisionShapes.clear();

	delete dynamicsWorld;
	delete solver;
	delete broadphase;
	delete dispatcher;
    delete collisionConfiguration;
}


void CubesDemo::reset_cube()
{
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    cubeBody->activate();
    btTransform trans = cubeBody->getWorldTransform();
    trans.setOrigin(btVector3(0,5,0));
    cubeBody->setWorldTransform(trans);

    cubeBody->applyTorque(btVector3(45*dist(randGen),
                                    45*dist(randGen),
                                    45*dist(randGen)));
}


void CubesDemo::render_cube() const
{
    glClearDepth(1.0f);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    colorShader->use();

    cubeBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    normalBuffer->bind(GL_ARRAY_BUFFER);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0);

    static int mvpId = colorShader->get_uniform_location("mvpMatrix");
    static int rotId = colorShader->get_uniform_location("normalRotationMatrix");
    static int colorId = colorShader->get_uniform_location("inColor");
    glUniform4f(colorId, 1.0, 0, 0, 1);

    glm::mat4 bulletMatrix;
    btTransform cubeTransform;
    cubeBody->getMotionState()->getWorldTransform(cubeTransform);
    cubeTransform.getOpenGLMatrix(&bulletMatrix[0][0]);

    glm::mat4 modelMat = bulletMatrix;
    glUniformMatrix4fv(mvpId, 1, GL_FALSE, &((projectionMatrix * viewMatrix * modelMat)[0][0]));
    glUniformMatrix4fv(rotId, 1, GL_FALSE, &((bulletMatrix)[0][0]));

    glDrawArrays(GL_TRIANGLES, 0, 3*2*6);
}
