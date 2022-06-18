//#pragma once
//
//static struct game
//{
//    std::unique_ptr<ecs::manager> m_GameMgr = std::make_unique<ecs::manager>();
//    int m_W = 1024;
//    int m_H = 800;
//
//} s_Game;
//
//#include "game_components.h"
//#include "game_systems.h"
//
//void InitializeGame(void) noexcept
//{
//    //
//    // Initialize global elements
//    //
//    std::srand(100);
//
//    //
//    // Register all the elements of the game
//    //
//    s_Game.m_GameMgr->RegisterComponents
//        < position
//        , velocity
//        , timer
//        , bullet
//        >();
//
//    s_Game.m_GameMgr->RegisterSystems
//        < update_movement         // Structural: No
//        , space_ship_logic        // Structural: Can Create Bullets
//        , bullet_logic            // Structural: Can Destroy Bullets and Ships
//        , render_ships            // Structural: No
//        , render_bullets          // Structural: No
//        , page_flip               // Structural: No
//        >();
//
//    //
//    // Generate a few random ships
//    //
//    auto& SpaceShipArchetype = s_Game.m_GameMgr->GetArchetype<position, velocity, timer>();
//    for (int i = 0; i < 1000; i++)
//    {
//        SpaceShipArchetype.CreateEntity([&](position& Position, velocity& Velocity, timer& Timer)
//            {
//                Position.m_Value.x = std::rand() % s_Game.m_W;
//                Position.m_Value.y = std::rand() % s_Game.m_H;
//
//                Velocity.m_Value.x = (std::rand() / (float)RAND_MAX) - 0.5f;
//                Velocity.m_Value.y = (std::rand() / (float)RAND_MAX) - 0.5f;
//                Velocity.m_Value.Normalize();
//
//                Timer.m_Value = (std::rand() / (float)RAND_MAX) * 8;
//            });
//    };
//}