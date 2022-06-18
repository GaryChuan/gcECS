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

struct position
{
    static constexpr auto typedef_v = ecs::component::type::data
    {
        .mName = "Position"
    };

    core::math::vector2 m_Value;
};

struct velocity
{
    static constexpr auto typedef_v = ecs::component::type::data
    {
        .mName = "Velocity"
    };

    core::math::vector2 m_Value;
};

struct timer
{
    static constexpr auto typedef_v = ecs::component::type::data
    {
        .mName = "Timer"
    };

    float m_Value{};
};

struct bullet
{
    static constexpr auto typedef_v = ecs::component::type::data
    {
        .mName = "Bullet"
    };

    ecs::component::entity m_ShipOwner;
};

//---------------------------------------------------------------------------------------
// SYSTEM
//---------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------

struct update_movement : ecs::system::base
{
    constexpr static auto   name_v = "update_movement";

    void operator()(position& Position, velocity& Velocity) const noexcept
    {
        Position.m_Value += Velocity.m_Value;

        // Bounce on edges
        if (Position.m_Value.x < 0)
        {
            Position.m_Value.x = 0;
            Velocity.m_Value.x = -Velocity.m_Value.x;
        }
        else if (Position.m_Value.x >= s_Game.m_W)
        {
            Position.m_Value.x = s_Game.m_W - 1;
            Velocity.m_Value.x = -Velocity.m_Value.x;
        }

        if (Position.m_Value.y < 0)
        {
            Position.m_Value.y = 0;
            Velocity.m_Value.y = -Velocity.m_Value.y;
        }
        else if (Position.m_Value.y >= s_Game.m_H)
        {
            Position.m_Value.y = s_Game.m_H - 1;
            Velocity.m_Value.y = -Velocity.m_Value.y;
        }
    }
};

//---------------------------------------------------------------------------------------

struct bullet_logic : ecs::system::base
{
    constexpr static auto   name_v = "bullet_logic";

    void operator()(ecs::component::entity& Entity, position& Position, timer& Timer, bullet& Bullet) const noexcept
    {
        // If I am dead because some other bullet killed me then there is nothing for me to do...
        if (Entity.isZombie()) return;

        // Update my timer
        Timer.m_Value -= 0.01f;
        if (Timer.m_Value <= 0)
        {
            mECSMgr.DeleteEntity(Entity);
            return;
        }

        // Check for collisions
        ecs::query query;
        query.AddMustHaveComponents<position>();

        mECSMgr.ForEach( 
            mECSMgr.Search(query), 
            [&](ecs::component::entity& E, position& Pos) noexcept -> bool
            {
                assert(Entity.isZombie() == false);

                // Our we checking against my self?
                if (&Entity == &E) return false;

                // Is this bullet or ship already dead?
                if (E.isZombie()) return false;

                // Are we colliding with our own ship?
                // If so lets just continue
                if (Bullet.m_ShipOwner.mValue == E.mValue) return false;

                constexpr auto distance_v = 3;
                if ((Pos.m_Value - Position.m_Value).GetMagnitudeSquared() < distance_v * distance_v)
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
    constexpr static auto   name_v = "space_ship_logic";

    using query = std::tuple<ecs::query::have_none_of<bullet>>;

    void operator()(ecs::component::entity& Entity, position& Position, timer& Time) const noexcept
    {
        if (Time.m_Value > 0)
        {
            Time.m_Value -= 0.01f;
            return;
        }

        ecs::query query;
        query.AddHaveNoneOfComponents<bullet>();
        
        mECSMgr.ForEach(
            mECSMgr.Search(query),
            [&](position& Pos) noexcept -> bool
            {
                // Don't shoot myself
                if (&Pos == &Position) return false;

                auto        Direction = Pos.m_Value - Position.m_Value;
                const auto  DistanceSquare = Direction.GetMagnitudeSquared();

                // Shoot a bullet if close enough
                constexpr auto min_distance_v = 30;
                if (DistanceSquare < min_distance_v * min_distance_v)
                {
                    Time.m_Value = 8;
                    auto& Archetype = mECSMgr.GetArchetype<position, velocity, timer, bullet>();
                    mECSMgr.CreateEntity([&](position& Pos, velocity& Vel, bullet& Bullet, timer& Timer)
                        {
                            Direction /= std::sqrt(DistanceSquare);
                            Vel.m_Value = Direction * 2.0f;
                            Pos.m_Value = Position.m_Value + Vel.m_Value;

                            Bullet.m_ShipOwner = Entity;
                            Timer.m_Value = 10;
                        }, Archetype);
                    return true;
                }
                return false;
            });
    }
};

//---------------------------------------------------------------------------------------

struct render_bullets : ecs::system::base
{
    constexpr static auto   name_v = "render_bullets";

    using query = std::tuple<ecs::query::must_have<bullet>>;

    void operator()(position& Position, velocity& Velocity) const noexcept
    {
        constexpr auto SizeX = 1;
        constexpr auto SizeY = SizeX * 3;
        glBegin(GL_TRIANGLES);
        glColor3f(1.0, 0.5, 0.0);
        glVertex2i(Position.m_Value.x + Velocity.m_Value.x * SizeY, Position.m_Value.y + Velocity.m_Value.y * SizeY);
        glVertex2i(Position.m_Value.x + Velocity.m_Value.y * SizeX, Position.m_Value.y - Velocity.m_Value.x * SizeX);
        glVertex2i(Position.m_Value.x - Velocity.m_Value.y * SizeX, Position.m_Value.y + Velocity.m_Value.x * SizeX);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct render_ships : ecs::system::base
{
    constexpr static auto   name_v = "render_ships";

    using query = std::tuple<ecs::query::have_none_of<bullet>>;

    void operator()(position& Position, timer& Timer) const noexcept
    {
        constexpr auto Size = 3;
        glBegin(GL_QUADS);
        if (Timer.m_Value > 0) glColor3f(1.0, 1.0, 1.0);
        else                   glColor3f(0.5, 1.0, 0.5);
        glVertex2i(Position.m_Value.x - Size, Position.m_Value.y - Size);
        glVertex2i(Position.m_Value.x - Size, Position.m_Value.y + Size);
        glVertex2i(Position.m_Value.x + Size, Position.m_Value.y + Size);
        glVertex2i(Position.m_Value.x + Size, Position.m_Value.y - Size);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct page_flip : ecs::system::base
{
    constexpr static auto   name_v = "page_flip";

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
        < position
        , velocity
        , timer
        , bullet
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
    auto& SpaceShipArchetype = s_Game.m_GameMgr->GetArchetype<position, velocity, timer>();
    for (int i = 0; i < 1000; i++)
    {
        s_Game.m_GameMgr->CreateEntity([&](position& Position, velocity& Velocity, timer& Timer)
            {
                Position.m_Value.x = std::rand() % s_Game.m_W;
                Position.m_Value.y = std::rand() % s_Game.m_H;

                Velocity.m_Value.x = (std::rand() / (float)RAND_MAX) - 0.5f;
                Velocity.m_Value.y = (std::rand() / (float)RAND_MAX) - 0.5f;
                Velocity.m_Value.Normalize();

                Timer.m_Value = (std::rand() / (float)RAND_MAX) * 8;
            }, SpaceShipArchetype);
    };
}

void Timer(int value)
{
    // Post re-paint request to activate display()
    glutPostRedisplay();

    // next timer call milliseconds later
    glutTimerFunc(15, Timer, 0);
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
    glutTimerFunc(0, Timer, 0);
    glutMainLoop();
}