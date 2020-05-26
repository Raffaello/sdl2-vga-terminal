#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SDL2/SDL.h>
#include <string>

class Environment : public ::testing::Environment
{
public:
    ~Environment() override {}

    static void setUp()
    {
        ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS));
        SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);
        SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);
    }

    static std::string getTitle()
    {
        return ::testing::UnitTest::GetInstance()->current_test_info()->name();
    }

    static void tearDown()
    {
        SDL_Quit();
    }

    // Override this to define how to set up the environment.
    void SetUp() override
    {
        Environment::setUp();
    }

    // Override this to define how to tear down the environment.
    void TearDown() override
    {
        Environment::tearDown();
    }
};
