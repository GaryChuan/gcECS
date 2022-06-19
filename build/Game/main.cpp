/******************************************************************************
filename: main.cpp
author: Gary Chuan gary.chuan@digipen.edu
Project: CS396 - Midterm Project
Description:
This file contains the implementation of the game itself, includiing its 
systems, components and initialization.
******************************************************************************/
#include "ecs.h" 

#define NOMINMAX
#include "Windows.h"

#define GLUT_STATIC_LIB
#include "GL/glut.h"

static struct game
{
    std::unique_ptr<ecs::manager> mGameMgr = std::make_unique<ecs::manager>();
    std::unique_ptr<ecs::manager> mParticleMgr = std::make_unique<ecs::manager>();
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
            .mName = "position"
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
            .mName = "velocity"
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
            .mName = "timer"
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
            .mName = "bullet"
        };

        ecs::component::entity mShipOwner;
    };

    struct particle
    {
        static constexpr auto typedef_v = ecs::component::type::data
        {
            .mName = "particle"
        };

        float mLifetime = 1.f;
        core::math::vector3 mStartColour{ 1.0f, 0.5f, 0.2f };
        core::math::vector3 mEndColour{ 0.f, 0.f, 0.f };
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
        ecs::component::entity& entity, 
        component::position& position, 
        component::timer& time, 
        component::bullet& bullet) const noexcept
    {
        // If I am dead because some other bullet killed me then there is nothing for me to do...
        if (entity.isZombie()) return;

        // Update my timer
        time -= 0.01f;
        if (time <= 0)
        {
            mECSMgr.DeleteEntity(entity);
            return;
        }

        // Check for collisions
        ecs::query query = ecs::make_query<
            ecs::query::must_have<component::position>,
            ecs::query::have_none_of<component::particle>>();

        mECSMgr.ForEach( 
            mECSMgr.Search(query), 
            [&](ecs::component::entity& entity2, component::position& position2) noexcept -> bool
            {
                assert(entity.isZombie() == false);

                // Our we checking against my self?
                if (&entity == &entity2) return false;

                // Is this bullet or ship already dead?
                if (entity2.isZombie()) return false;

                // Are we colliding with our own ship?
                // If so lets just continue
                if (bullet.mShipOwner.mID == entity2.mID) return false;

                constexpr auto distance_v = 3;
                if ((position2 - position).GetMagnitudeSquared() < distance_v * distance_v)
                {
                    auto& particleArchetype = s_Game.mParticleMgr->GetArchetype<component::position, component::velocity, component::particle, component::timer>();
                    for (int i = 0; i < 50; i++)
                    {
                        particleArchetype.CreateEntity([&](
                            component::position& particlePos,
                            component::velocity& particleVel,
                            component::particle& particle,
                            component::timer& particleAge)
                            {
                                particlePos = position2;

                                particleVel.x = (std::rand() / (float)RAND_MAX) - 0.5f;
                                particleVel.y = (std::rand() / (float)RAND_MAX) - 0.5f;
                                particleVel.Normalize();

                                particle.mLifetime = std::min((std::rand() / (float)RAND_MAX) * 8, 2.f);
                                particleAge = 0.f;
                            });
                    }

                     mECSMgr.DeleteEntity(entity);
                     mECSMgr.DeleteEntity(entity2);
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
            [&](component::position& position2) noexcept -> bool
            {
                // Don't shoot myself
                if (&position2 == &position) return false;

                auto        direction  = position2 - position;
                const auto  distanceSq = direction.GetMagnitudeSquared();

                // Shoot a bullet if close enough
                constexpr auto min_distance_v = 30;
                if (distanceSq < min_distance_v * min_distance_v)
                {
                    time = 8;
                    auto& bulletArchetype = mECSMgr.GetArchetype<component::position, component::velocity, component::timer, component::bullet>();
                    bulletArchetype.CreateEntity([&](component::position& pos, component::velocity& vel, component::bullet& bullet, component::timer& time)
                        {
                            direction /= std::sqrt(distanceSq);
                            vel = direction * 2.0f;
                            pos = position  + vel;

                            bullet.mShipOwner = entity;
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
        else          glColor3f(0.5, 1.0, 0.5);
        glVertex2i(position.x - Size, position.y - Size);
        glVertex2i(position.x - Size, position.y + Size);
        glVertex2i(position.x + Size, position.y + Size);
        glVertex2i(position.x + Size, position.y - Size);
        glEnd();
    }
};

//---------------------------------------------------------------------------------------

struct particles_logic : ecs::system::base
{
    constexpr static auto name = "particles_logic";

    using query = std::tuple<ecs::query::must_have<component::particle>>;

    void operator()(
        ecs::component::entity& entity,
        component::particle& particle,
        component::timer& age,
        component::position& position,
        component::velocity& vel) const noexcept
    {
        if (entity.isZombie()) return;

        age += 0.1f;

        if (age >= particle.mLifetime)
        {
            mECSMgr.DeleteEntity(entity);
            return;
        }
    }
};

//---------------------------------------------------------------------------------------

struct render_particles : ecs::system::base
{
    constexpr static auto name = "render_particles";

    using query = std::tuple<ecs::query::must_have<component::particle>>;

    void operator()(component::particle& particle, component::position& position, component::timer& age) const noexcept
    {
        constexpr auto Size = 2;
        glBegin(GL_QUADS);

        auto colour = core::math::Lerp(particle.mStartColour, particle.mEndColour, age  / particle.mLifetime);
        glColor3f(colour.r, colour.g, colour.b);
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
    s_Game.mGameMgr->RegisterComponents
        < component::position
        , component::velocity
        , component::timer
        , component::bullet
        >();

    s_Game.mGameMgr->RegisterSystems
        < update_movement         // Structural: No
        , space_ship_logic        // Structural: Can Create Bullets
        , bullet_logic            // Structural: Can Destroy Bullets and Ships and Create Particles
        , render_ships            // Structural: No
        , render_bullets          // Structural: No
        >();

    //
    // Register all elements of the particle manager
    //
    s_Game.mParticleMgr->RegisterComponents
        < component::position
        , component::velocity
        , component::timer
        , component::particle
        >();

    s_Game.mParticleMgr->RegisterSystems
        < update_movement   // Structural: No
        , particles_logic   // Structural: Can Destroy Particles
        , render_particles  // Structural: No
        , page_flip         // Structural: No
        >();

    //
    // Generate a few random ships
    //
    auto& spaceShipArchetype = s_Game.mGameMgr->GetArchetype<component::position, component::velocity, component::timer>();
    for (int i = 0; i < 90000; i++)
    {
        spaceShipArchetype.CreateEntity([&](component::position& position, component::velocity& vel, component::timer& time)
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
            s_Game.mGameMgr->Run();
            s_Game.mParticleMgr->Run();
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