#pragma once


#include "Environment.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <algorithm>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>


class SnapshotEnvironment : public Environment
{
    // Override this to define how to set up the environment.
    void SetUp() override
    {
        Environment::setUp();
        ASSERT_NE(0, IMG_Init(IMG_INIT_PNG));
    }

    // Override this to define how to tear down the environment.
    void TearDown() override
    {
        IMG_Quit();
        Environment::tearDown();
    }
};

std::string generateSnapshotFilename()
{
    std::string snapshotFilename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
    snapshotFilename += '.';
    snapshotFilename += ::testing::UnitTest::GetInstance()->current_test_info()->name();
    snapshotFilename += ".png";
    std::replace(snapshotFilename.begin(), snapshotFilename.end(), '/', '-');

    return snapshotFilename;
}

SDL_Surface* getScreenshot(SDL_Window* window, SDL_Renderer* renderer)
{
    int w = 0;
    int h = 0;
    constexpr uint32_t format = SDL_PIXELFORMAT_ABGR8888;
    constexpr int depth = 32;

    SDL_GetWindowSize(window, &w, &h);
    SDL_Surface* snapshot = SDL_CreateRGBSurfaceWithFormat(0, w, h, depth, format);

    if (SDL_RenderReadPixels(renderer, nullptr, format, snapshot->pixels, snapshot->pitch) != 0) {
        GTEST_LOG_(ERROR) << "Unable to take screenshoot:" << SDL_GetError();
    }

    return snapshot;
}

void snapShotTest(SDL_Window* window, SDL_Renderer* renderer, const std::string& snapshotFilename)
{
    SDL_Surface* snapshot = getScreenshot(window, renderer);

#ifdef TEST_DUMP_SNAPSHOT
    GTEST_LOG_(INFO) << "Dumping snapshot: " << snapshotFilename;
    IMG_SavePNG(snapshot, ("snapshot/" + snapshotFilename).c_str());
#else
    SDL_Surface* image = IMG_Load(("snapshot/" + snapshotFilename).c_str());
    ASSERT_FALSE(NULL == image);
    ASSERT_FALSE(NULL == image->format);
    // SDL_PIXELFORMAT_BGRA32 on CI macos software rendering... cannot investigate further so comment out the check.
    EXPECT_EQ(image->format->format, snapshot->format->format);
    EXPECT_EQ(image->format->BytesPerPixel, snapshot->format->BytesPerPixel);
    EXPECT_EQ(image->pitch, snapshot->pitch);
    EXPECT_EQ(image->w, snapshot->w);
    EXPECT_EQ(image->h, snapshot->h);

    if (SDL_LockSurface(image) != 0) {
        GTEST_LOG_(ERROR) << "unable to access image" << SDL_GetError();
    }

    if (SDL_LockSurface(snapshot) != 0) {
        GTEST_LOG_(ERROR) << "unable to access snapshot" << SDL_GetError();
    }

    int size = image->pitch * image->h;
    EXPECT_EQ(0, std::memcmp(image->pixels, snapshot->pixels, size));
    if (::testing::Test::HasFailure()) {
        // Dump wrong result for inspection.
        GTEST_LOG_(ERROR) << "An Error has occorred. Dumping screenshot...";
        IMG_SavePNG(snapshot, ("snapshot/error_" + snapshotFilename).c_str());
    }

    SDL_UnlockSurface(image);
    SDL_UnlockSurface(snapshot);
    SDL_FreeSurface(image);


#endif
    SDL_FreeSurface(snapshot);
}

int snapshotMain(int argc, char** argv)
{
#ifdef TEST_DUMP_SNAPSHOT
    std::cout
            << "******************************************************************************************************" << std::endl
            << "*** WARNING !!! : DUMPING SCREENSHOT, IF IT IS NOT INTENDED REMOVE THE DEFINE 'TEST_DUMP_SNAPSHOT' ***" << std::endl
            << "***               AND RECOMPILE AGAIN.                                                             ***" << std::endl
            << "*** WARNING !!! : THE FILE ARE AVAILABLE IN THE SUB-DIRECTORY 'snapshot' AND WILL BE REUSED FOR    ***" << std::endl
            << "***               VERIFYING WHEN NOT DUMPING                                                       ***" << std::endl
            << "******************************************************************************************************"
            << std::endl;
#endif
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment());
    return RUN_ALL_TESTS();
}
