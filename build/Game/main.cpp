#include "ecs.h" 

#define NOMINMAX
#include "Windows.h"

#define GLUT_STATIC_LIB
#include "GL/glut.h"

static struct game
{
    std::unique_ptr<ecs::manager> m_GameMgr = std::make_unique<ecs::manager>();
    int m_W = 1024;
    int m_H = 800;

} s_Game;

//---------------------------------------------------------------------------------------
// COMPONENTS
//---------------------------------------------------------------------------------------

namespace component
{
    struct position : core::math::vector2
    {
        static constexpr auto typedef_v = ecs::component::type::data
        {
            .mName = "Position"
        };

        vector2& operator = (const vector2& rhs) noexcept
        {
            x = rhs.x;
            y = rhs.y;
            return *this;
        }
    };

    struct velocity : core::math::vector2
    {
        static constexpr auto typedef_v = ecs::component::type::data
        {
            .mName = "Velocity"
        };

        vector2& operator = (const vector2& rhs) noexcept
        {
            x = rhs.x;
            y = rhs.y;
            return *this;
        }
    };

    struct timer : core::math::arithmetic_type<float>
    {
        static constexpr auto typedef_v = ecs::component::type::data
        {
            .mName = "Timer"
        };

        core::math::arithmetic_type<float>& operator = (const float& rhs) noexcept
        {
            mValue = rhs;
            return *this;
        }
    };

    struct bullet
    {
        static constexpr auto typedef_v = ecs::component::type::data
        {
            .mName = "Bullet"
        };

        ecs::component::entity m_ShipOwner;
    };
}


//---------------------------------------------------------------------------------------
// SYSTEM
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

struct update_movement : ecs::system::base
{
    constexpr static auto name = "update_movement";

    void operator()(component::position& position, component::velocity& vel) const noexcept
    {
        position += vel;

        // Bounce on edges
        if (position.x < 0)
        {
            position.x = 0;
            vel.x = -vel.x;
        }
        else if (position.x >= s_Game.m_W)
        {
            position.x = s_Game.m_W - 1;
            vel.x = -vel.x;
        }

        if (position.y < 0)
        {
            position.y = 0;
            vel.y = -vel.y;
        }
        else if (position.y >= s_Game.m_H)
        {
            position.y = s_Game.m_H - 1;
            vel.y = -vel.y;
        }
    }
};

//---------------------------------------------------------------------------------------

struct bullet_logic : ecs::system::base
{
    constexpr static auto name = "bullet_logic";

    void operator()(
        ecs::component::entity& Entity, 
        component::position& position, 
        component::timer& time, 
        component::bullet& Bullet) const noexcept
    {
        // If I am dead because some other bullet killed me then there is nothing for me to do...
        if (Entity.isZombie()) return;

        // Update my timer
        time -= 0.01f;
        if (time <= 0)
        {
            mECSMgr.DeleteEntity(Entity);
            return;
        }

        // Check for collisions
        ecs::query query = ecs::make_query<ecs::query::must_have<component::position>>();

        mECSMgr.ForEach( 
            mECSMgr.Search(query), 
            [&](ecs::component::entity& E, component::position& Pos) noexcept -> bool
            {
                assert(Entity.isZombie() == false);

                // Our we checking against my self?
                if (&Entity == &E) return false;

                // Is this bullet or ship already dead?
                if (E.isZombie()) return false;

                // Are we colliding with our own ship?
                // If so lets just continue
                if (Bullet.m_ShipOwner.mID == E.mID) return false;

                constexpr auto distance_v = 3;
                if ((Pos - position).GetMagnitudeSquared() < distance_v * distance_v)
                {
                     mECSMgr.DeleteEntity(Entity);
                     mECSMgr.DeleteEntity(E);
                    return true;
                }

                return false;
            });
    }
};

//---------------------------------------------------------------------------------------

struct space_ship_logic : ecs::system::base
{
    constexpr static auto name = "space_ship_logic";

    using query = std::tuple<ecs::query::have_none_of<component::bullet>>;

    void operator()(
        ecs::component::entity& entity, 
        component::position& position,
        component::timer& time) const noexcept
    {
        if (time > 0)
        {
            time -= 0.01f;
            return;
        }

        ecs::query query = ecs::make_query<ecs::query::have_none_of<component::bullet>>();
        
        mECSMgr.ForEach(
            mECSMgr.Search(query),
            [&](component::position& pos) noexcept -> bool
            {
                // Don't shoot myself
                if (&pos == &position) return false;

                auto        direction  = pos - position;
                const auto  distanceSq = direction.GetMagnitudeSquared();

                // Shoot a bullet if close enough
                constexpr auto min_distance_v = 30;
                if (distanceSq < min_distance_v * min_distance_v)
                {
                    time = 8;
                    auto& archetype = mECSMgr.GetArchetype<component::position, component::velocity, component::timer, component::bullet>();
                    archetype.CreateEntity([&](component::position& pos, component::velocity& vel, component::bullet& bullet, component::timer& time)
                        {
                            direction /= std::sqrt(distanceSq);
                            vel = direction * 2.0f;
                            pos = position  +vel;

                            bullet.m_ShipOwner = entity;
                            time = 10;
                        });
                    return true;
                }
                return false;
            });
    }
};

//---------------------------------------------------------------------------------------

struct render_bullets : ecs::system::base
{
    constexpr static auto name = "render_bullets";

    using query = std::tuple<ecs::query::must_have<component::bullet>>;

    void operator()(component::position& position, component::velocity& vel) const noexcept
    {
        constexpr auto SizeX = 1;
        constexpr auto SizeY = SizeX * 3;
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.5, 0.0);
        glVertex2i(position.x + vel.x * SizeY, position.y + vel.y * SizeY);
        glVertex2i(position.x + vel.y * SizeX, position.y - vel.x * SizeX);
        glVertex2i(position.x - vel.y * SizeX, position.y + vel.x * SizeX);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct render_ships : ecs::system::base
{
    constexpr static auto name = "render_ships";

    using query = std::tuple<ecs::query::have_none_of<component::bullet>>;

    void operator()(component::position& position, component::timer& time) const noexcept
    {
        constexpr auto Size = 3;
        glBegin(GL_QUADS);
        if (time > 0) glColor3f(1.0, 1.0, 1.0);
        else           glColor3f(0.5, 1.0, 0.5);
        glVertex2i(position.x - Size, position.y - Size);
        glVertex2i(position.x - Size, position.y + Size);
        glVertex2i(position.x + Size, position.y + Size);
        glVertex2i(position.x + Size, position.y - Size);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct page_flip : ecs::system::base
{
    constexpr static auto name = "page_flip";

    __inline
        void OnUpdate(void) noexcept
    {
        glutSwapBuffers();
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

void InitializeGame(void) noexcept
{
    //
    // Initialize global elements
    //
    std::srand(100);

    //
    // Register all the elements of the game
    //
    s_Game.m_GameMgr->RegisterComponents
        < component::position
        , component::velocity
        , component::timer
        , component::bullet
        >();

    s_Game.m_GameMgr->RegisterSystems
        < update_movement         // Structural: No
        , space_ship_logic        // Structural: Can Create Bullets
        , bullet_logic            // Structural: Can Destroy Bullets and Ships
        , render_ships            // Structural: No
        , render_bullets          // Structural: No
        , page_flip               // Structural: No
        >();

    //
    // Generate a few random ships
    //
    auto& SpaceShipArchetype = s_Game.m_GameMgr->GetArchetype<component::position, component::velocity, component::timer>();
    for (int i = 0; i < 1000; i++)
    {
        SpaceShipArchetype.CreateEntity([&](component::position& position, component::velocity& vel, component::timer& time)
            {
                position.x = std::rand() % s_Game.m_W;
                position.y = std::rand() % s_Game.m_H;

                vel.x = (std::rand() / (float)RAND_MAX) - 0.5f;
                vel.y = (std::rand() / (float)RAND_MAX) - 0.5f;
                vel.Normalize();

                time = (std::rand() / (float)RAND_MAX) * 8;
            });
    };
}

void time(int value)
{
    // Post re-paint request to activate display()
    glutPostRedisplay();

    // next timer call milliseconds later
    glutTimerFunc(15, time, 0);
}

int main(int argc, char** argv)
{
	_CrtDumpMemoryLeaks();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

    InitializeGame();

    glutInitWindowSize(s_Game.m_W, s_Game.m_H);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE);
    glutCreateWindow("My Game");
    glutDisplayFunc([]
        {
            s_Game.m_GameMgr->Run();
        });
    glutReshapeFunc([](int w, int h)
        {
            s_Game.m_W = w;
            s_Game.m_H = h;
            glViewport(0, 0, w, h);
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(0, w, 0, h, -1, 1);
            glScalef(1, -1, 1);
            glTranslatef(0, -h, 0);
        });
    glutTimerFunc(0, time, 0);
    glutMainLoop();
}