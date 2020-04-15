#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>

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

TEST(VgaTerminal, SetViewportNull)
{
	using ::testing::StartsWith;
	using ::testing::EndsWith;

	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	testing::internal::CaptureStderr();
	auto r = term.getViewport();
	term.setViewPort(0, 0, 0, 0);
	std::string output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));
	
	auto e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	term.setViewPort(r);
	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	
	testing::internal::CaptureStderr();
	term.setViewPort(0, 0, 1, 0);
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	
	testing::internal::CaptureStderr();
	term.setViewPort(0, 0, 0, 1);
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	SDL_Quit();
}

TEST(VgaTerminal, SetViewport1)
{
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	auto r = term.getViewport();
	term.setViewPort(0, 0, 1, 1);
	auto e = term.getViewport();
	EXPECT_EQ(0, e.x);
	EXPECT_EQ(0, e.y);
	EXPECT_EQ(1, e.w);
	EXPECT_EQ(1, e.h);
	term.write("X", 15, 0);
	auto tc = term.at(0, 0);
	EXPECT_EQ(0, tc.c);
	EXPECT_EQ(0, tc.col);
	EXPECT_EQ(0, tc.bgCol);

	SDL_Quit();
}

TEST(VgaTerminal, SetViewport2)
{
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	auto r = term.getViewport();
	term.setViewPort(10, 10, 10, 10);
	auto e = term.getViewport();
	EXPECT_EQ(10, e.x);
	EXPECT_EQ(10, e.y);
	EXPECT_EQ(10, e.w);
	EXPECT_EQ(10, e.h);

	term.writeXY(5, 5, "Test", 15, 0);
	auto tc = term.at(10 + 5, 10 + 5);
	EXPECT_EQ('T', tc.c);
	EXPECT_EQ(15, tc.col);
	EXPECT_EQ(0, tc.bgCol);

	SDL_Quit();
}

TEST(VgaTerminal, moveCursorCircle) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.gotoXY(10, 10);
	term.moveCursorLeft();
	EXPECT_EQ(9, term.getX());
	EXPECT_EQ(10, term.getY());
	term.moveCursorDown();
	EXPECT_EQ(9, term.getX());
	EXPECT_EQ(11, term.getY());
	term.moveCursorRight();
	EXPECT_EQ(10, term.getX());
	EXPECT_EQ(11, term.getY());
	term.moveCursorUp();
	EXPECT_EQ(10, term.getX());
	EXPECT_EQ(10, term.getY());
	SDL_Quit();
}

TEST(VgaTerminal, moveCursorBorder) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.gotoXY(79, 23);
	term.moveCursorRight();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(24, term.getY());
	term.moveCursorDown();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(24, term.getY());
	term.moveCursorLeft();
	EXPECT_EQ(79, term.getX());
	EXPECT_EQ(23, term.getY());
	term.moveCursorUp();
	EXPECT_EQ(79, term.getX());
	EXPECT_EQ(22, term.getY());
	SDL_Quit();
}

TEST(VgaTerminal, ViewportMoveCursorBorder) {
	SDL_Init(SDL_INIT_VIDEO);
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	term.setViewPort(10, 10, 10, 10);
	term.gotoXY(9, 2);
	term.moveCursorRight();
	EXPECT_EQ(10, term.getX());
	EXPECT_EQ(13, term.getY());
	term.moveCursorLeft();
	EXPECT_EQ(19, term.getX());
	EXPECT_EQ(12, term.getY());
	
	SDL_Quit();
}
#endif

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}
