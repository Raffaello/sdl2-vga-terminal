#include <gtest/gtest.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>

std::string generateSnapshotFilename() {
	std::string snapshotFilename = ::testing::UnitTest::GetInstance()->current_test_info()->test_case_name();
	snapshotFilename += '.';
	snapshotFilename += ::testing::UnitTest::GetInstance()->current_test_info()->name();
	snapshotFilename += ".png";
	
	return snapshotFilename;
}

SDL_Surface* getScreenshot(const VgaTerminal &term) {
	int w = 0, h = 0;
	const uint32_t format = SDL_PIXELFORMAT_ABGR8888;

	SDL_GetWindowSize(term.getWindow(), &w, &h);
	SDL_Surface* snapshot = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, format);

	if (SDL_RenderReadPixels(term.getRenderer(), NULL, format, snapshot->pixels, snapshot->pitch) != 0) {
		GTEST_LOG_(ERROR) << "Unable to take screenshoot:" << SDL_GetError();
	}

	return snapshot;
}

#ifndef TEST_DUMP_SNAPSHOT
TEST(VgaTerminal, CannotInit) {
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
}

TEST(VgaTerminal, HelloWorld_Window) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	
	std::string title = SDL_GetWindowTitle(term.getWindow());
	ASSERT_EQ(title, termTitle);
	
	SDL_Quit();
}

TEST(VgaTerminal, HelloWorld_Text) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.write(termTitle, 7, 0);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), term.getXY());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), VgaTerminal::position_t(term.getX(), term.getY()));
	ASSERT_EQ('T', term.at(6, 0).c);

	SDL_Quit();
}

TEST(VgaTerminal, ScrollDown) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.writeXY(VgaTerminal::mode3.tw - 1, VgaTerminal::mode3.th - 1, termTitle, 7, 1);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());
	
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength - 1, VgaTerminal::mode3.th - 1), term.getXY());
	ASSERT_EQ('H', term.at(VgaTerminal::mode3.tw - 1, VgaTerminal::mode3.th - 2).c);
	ASSERT_EQ('T', term.at(5, VgaTerminal::mode3.th - 1).c);
	
	SDL_Quit();
}
#endif

TEST(VgaTerminal, Snapshot) {
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, 0, -1, 0);
	std::string snapshotFilename = generateSnapshotFilename();
	
	for (int i = 0; i < 256; i++) {
		term.write((char)i, i, 255 - i);
	}

	term.writeXY(32, 11, "ษอออออออออออออออป", 11, 4);
	term.writeXY(32, 12, "บ Hello World!! บ", 11, 4);
	term.writeXY(32, 13, "ศอออออออออออออออผ", 11, 4);
	term.render();
	SDL_Delay(1000);
	SDL_Surface* snapshot = getScreenshot(term);

#ifdef TEST_DUMP_SNAPSHOT
	GTEST_LOG_(INFO) << "Dumping snapshot: " << snapshotFilename;
	IMG_SavePNG(snapshot, ("snapshot/" + snapshotFilename).c_str());
	SDL_FreeSurface(snapshot);
#else
	SDL_Surface* image = IMG_Load(("snapshot/" + snapshotFilename).c_str());
	
	ASSERT_EQ(image->format->format, snapshot->format->format);
	ASSERT_EQ(image->format->BytesPerPixel, snapshot->format->BytesPerPixel);
	ASSERT_EQ(image->pitch, snapshot->pitch);
	ASSERT_EQ(image->w, snapshot->w);
	ASSERT_EQ(image->h, snapshot->h);
	
	if (SDL_LockSurface(image) != 0) {
		GTEST_LOG_(ERROR) << "unable to access image" << SDL_GetError();
	}

	if (SDL_LockSurface(snapshot) != 0) {
		GTEST_LOG_(ERROR) << "unable to access snapshot" << SDL_GetError();
	}

	int size = image->pitch * image->h;
	ASSERT_EQ(0, SDL_memcmp(image->pixels, snapshot->pixels, size));

	SDL_UnlockSurface(image);
	SDL_UnlockSurface(snapshot);
	SDL_FreeSurface(image);
	SDL_FreeSurface(snapshot);
#endif
	
	IMG_Quit();
	SDL_Quit();
}

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
