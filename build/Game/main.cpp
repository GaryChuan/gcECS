#include "test_ecs.h"
#include "test_traits_function.h"

#define NOMINMAX
#include "Windows.h"


#define GLUT_STATIC_LIB
#include "GL/glut.h"

//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>
struct Comp1
{
	static constexpr auto typedef_v = ecs::component::type::data
	{
		.mName = "Comp1"
	};
};

struct Comp2
{
	static constexpr auto typedef_v = ecs::component::type::data
	{
		.mName = "Comp2"
	};
};

template <typename T>
void printType(const T&)
{
    std::cout << typeid(T).name() << std::endl;
}

struct Sys1 : public ecs::system::base
{
	void operator()(Comp1&, Comp2&) const noexcept { printType(*this); }
};

struct Sys2 : public ecs::system::base
{
	void operator()(Comp1&, Comp2&) const noexcept { printType(*this); }
};

struct page_flip : public ecs::system::base
{
    __inline
        void OnUpdate(void) noexcept
    {
        std::cout << "Page Flip" << std::endl;
        glutSwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void operator()(Comp1&) const noexcept { std::cout << "Fake Page Flip" << std::endl; }
};

void timer(int value)
{
    // Post re-paint request to activate display()
    glutPostRedisplay();

    // next timer call milliseconds later
    glutTimerFunc(15, timer, 0);
}

int main(int argc, char** argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    // test::ecs();
    // s_Game.m_GameMgr->Run();
    // xcore::Init("ECS Example");
    static ecs::manager manager;
    manager.RegisterComponents<Comp1, Comp2>();
    manager.RegisterSystems<Sys1, Sys2, page_flip>();
    auto& archetype = manager.GetArchetype<Comp1, Comp2>();

    for (int i = 0; i < 100; ++i)
    {
        manager.CreateEntity([&i](Comp1&, Comp2&) {}, archetype);
    }

    glutInitWindowSize(800, 600);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow("My Game");
    glutDisplayFunc([]
        {
            manager.    Run();
            // s_Game.m_GameMgr->Run();
        });
    glutReshapeFunc([](int w, int h)
        {
            // s_Game.m_W = w;
            // s_Game.m_H = h;
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, 0, h, -1, 1);
            glScalef(1, -1, 1);
            glTranslatef(0, -h, 0);
        });
    glutTimerFunc(0, timer, 0);
    glutMainLoop();

    // xcore::Kill();

	test::function_traits();
}