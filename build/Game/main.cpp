#include "test_ecs.h"
#include "test_traits_function.h"

#define NOMINMAX
#include "Windows.h"


#define GLUT_STATIC_LIB
#include "GL/glut.h"

//#define _CRTDBG_MAP_ALLOC
//#include <cstdlib>
//#include <crtdbg.h>




int main(int argc, char** argv)
{
	/*_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);*/

    // xcore::Init("ECS Example");
    
    glutInitWindowSize(800, 600);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow("My Game");
    glutDisplayFunc([]
        {

            test::ecs();
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
    // glutTimerFunc(0, timer, 0);
    glutMainLoop();

    // xcore::Kill();

	// test::function_traits();
}