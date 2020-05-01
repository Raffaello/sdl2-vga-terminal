#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>
#include <algorithm>


std::string generateSnapshotFilename()
{
	std::string snapshotFilename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
	snapshotFilename += '.';
	snapshotFilename += ::testing::UnitTest::GetInstance()->current_test_info()->name();
	snapshotFilename += ".png";
	std::replace(snapshotFilename.begin(), snapshotFilename.end(), '/', '-');
	return snapshotFilename;
}

SDL_Surface* getScreenshot(const VgaTerminal& term)
{
	int w = 0, h = 0;
	const uint32_t format = SDL_PIXELFORMAT_ABGR8888;

	SDL_GetWindowSize(term.getWindow(), &w, &h);
	SDL_Surface* snapshot = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, format);

	if (SDL_RenderReadPixels(term.getRenderer(), NULL, format, snapshot->pixels, snapshot->pitch) != 0) {
		GTEST_LOG_(ERROR) << "Unable to take screenshoot:" << SDL_GetError();
	}

	return snapshot;
}

void snapShotTest(VgaTerminal& term, const std::string& snapshotFilename)
{
	term.render();

	SDL_Surface* snapshot = getScreenshot(term);

#ifdef TEST_DUMP_SNAPSHOT
	GTEST_LOG_(INFO) << "Dumping snapshot: " << snapshotFilename;
	IMG_SavePNG(snapshot, ("snapshot/" + snapshotFilename).c_str());
#else
	SDL_Surface* image = IMG_Load(("snapshot/" + snapshotFilename).c_str());
	ASSERT_FALSE(NULL == image);
	ASSERT_FALSE(NULL == image->format);
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
	EXPECT_EQ(0, SDL_memcmp(image->pixels, snapshot->pixels, size));

	SDL_UnlockSurface(image);
	SDL_UnlockSurface(snapshot);
	SDL_FreeSurface(image);
#endif
	SDL_FreeSurface(snapshot);
}

TEST(VgaTerminal, Snapshot)
{
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO));
	ASSERT_NE(0, IMG_Init(IMG_INIT_PNG));
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, 0, -1, 0);
	term.showCursor = false;
	std::string snapshotFilename = generateSnapshotFilename();

	for (int i = 0; i < 256; i++) {
		term.write(static_cast<char>(i), i, 255 - i);
	}

	term.writeXY(32, 11, "ษอออออออออออออออป", 14, 1);
	term.writeXY(32, 12, "บ Hello World!! บ", 14, 1);
	term.writeXY(32, 13, "ศอออออออออออออออผ", 14, 1);
	
	snapShotTest(term, snapshotFilename);

	IMG_Quit();
	SDL_Quit();
}

class  CursorShapeTests: public ::testing::TestWithParam<VgaTerminal::CURSOR_MODE> {};
TEST_P(CursorShapeTests, CursorShapeSnapshot)
{
	VgaTerminal::CURSOR_MODE cursorMode = GetParam();
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS));
	ASSERT_NE(0, IMG_Init(IMG_INIT_PNG));
	std::string snapshotFilename = generateSnapshotFilename();
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name(); 
	VgaTerminal term = VgaTerminal(title, 0, -1, 0);
	term.cursor_mode = cursorMode;
	term.write(title, 7, 0);
	
	snapShotTest(term, snapshotFilename);

	IMG_Quit();
	SDL_Quit();
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	CursorShapeTests,
	::testing::Values(
		VgaTerminal::CURSOR_MODE::CURSOR_MODE_NORMAL,
		VgaTerminal::CURSOR_MODE::CURSOR_MODE_FAT,
		VgaTerminal::CURSOR_MODE::CURSOR_MODE_BLOCK,
		VgaTerminal::CURSOR_MODE::CURSOR_MODE_VERTICAL
	)
);

int main(int argc, char** argv) {
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
	return RUN_ALL_TESTS();
}
