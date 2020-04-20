#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>


class Environment : public ::testing::Environment {
public:
	~Environment() override {}

	// Override this to define how to set up the environment.
	void SetUp() override
	{
		ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO));
	}

	// Override this to define how to tear down the environment.
	void TearDown() override
	{
		SDL_Quit();
	}
};

::testing::Environment* env;

#ifndef TEST_DUMP_SNAPSHOT
TEST(VgaTerminal, CannotInit) {
	env->TearDown();
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
	env->SetUp();
}

TEST(VgaTerminal, HelloWorldWindow) {
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	
	std::string title = SDL_GetWindowTitle(term.getWindow());
	ASSERT_EQ(title, termTitle);
}

TEST(VgaTerminal, HelloWorldText) {
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.write(termTitle, 7, 0);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), term.getXY());
	ASSERT_EQ(VgaTerminal::position_t(termTitleLength, 0), VgaTerminal::position_t(term.getX(), term.getY()));
	ASSERT_EQ('T', term.at(6, 0).c);
}

TEST(VgaTerminal, ScrollDown) {
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.writeXY(term.getMode().tw - 1, term.getMode().th - 1, termTitle, 7, 1);
	uint8_t termTitleLength = static_cast<uint8_t>(termTitle.size());

	ASSERT_EQ(VgaTerminal::position_t(termTitleLength - 1, term.getMode().th - 1), term.getXY());
	ASSERT_EQ('H', term.at(term.getMode().tw - 1, term.getMode().th - 2).c);
	ASSERT_EQ('T', term.at(5, term.getMode().th - 1).c);
}

TEST(VgaTerminal, SetViewportNull)
{
	using ::testing::StartsWith;
	using ::testing::EndsWith;

	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	testing::internal::CaptureStderr();
	auto r = term.getViewport();
	EXPECT_FALSE(term.setViewPort(0, 0, 0, 0));
	std::string output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));
	
	auto e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	EXPECT_TRUE(term.setViewPort(r));
	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	
	testing::internal::CaptureStderr();
	EXPECT_FALSE(term.setViewPort(0, 0, 1, 0));
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
	
	testing::internal::CaptureStderr();
	EXPECT_FALSE(term.setViewPort(0, 0, 0, 1));
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	EXPECT_EQ(r.x, e.x);
	EXPECT_EQ(r.y, e.y);
	EXPECT_EQ(r.w, e.w);
	EXPECT_EQ(r.h, e.h);
}

class SetViewportParameterTests : public ::testing::TestWithParam<std::tuple<int, int, int, int, bool>>{};

TEST_P(SetViewportParameterTests, SetVieport)
{
	int x = std::get<0>(GetParam());
	int y = std::get<1>(GetParam());
	int w = std::get<2>(GetParam());
	int h = std::get<3>(GetParam());
	int autoScroll = std::get<4>(GetParam());
		
	VgaTerminal term = VgaTerminal("SetViewport Test", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = autoScroll;

	EXPECT_TRUE(term.setViewPort(x, y, w, h));
	auto e = term.getViewport();
	EXPECT_EQ(x, e.x);
	EXPECT_EQ(y, e.y);
	EXPECT_EQ(w, e.w);
	EXPECT_EQ(h, e.h);
	term.write("X", 15, 1);
	auto tc = term.at(0, 0);
	EXPECT_EQ('X', tc.c);
	EXPECT_EQ(15, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

INSTANTIATE_TEST_SUITE_P(
	DISABLED_VgaTerminal,
	SetViewportParameterTests,
	::testing::Values(
		std::make_tuple(0, 0, 1, 1, false),
		std::make_tuple(10, 10, 10, 10, true)
	)
);

TEST(VgaTerminal, SetViewport1)
{
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

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
}

TEST(VgaTerminal, SetViewport2)
{
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);

	term.setViewPort(10, 10, 10, 10);
	auto e = term.getViewport();
	EXPECT_EQ(10, e.x);
	EXPECT_EQ(10, e.y);
	EXPECT_EQ(10, e.w);
	EXPECT_EQ(10, e.h);

	term.writeXY(5, 5, "Test", 15, 1);
	auto tc = term.at(5, 5);
	EXPECT_EQ('T', tc.c);
	EXPECT_EQ(15, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

TEST(VgaTerminal, moveCursorCircle)
{
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
}

TEST(VgaTerminal, moveCursorBorder)
{
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
}

TEST(VgaTerminal, ViewportMoveCursorBorder)
{
	std::string termTitle = "Hello Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	term.setViewPort(10, 10, 10, 10);
	term.gotoXY(9, 2);
	term.moveCursorRight();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(3, term.getY());
	term.moveCursorLeft();
	EXPECT_EQ(9, term.getX());
	EXPECT_EQ(2, term.getY());
}

TEST(VgaTerminal, NoAutoScroll)
{
	std::string termTitle = "Test";
	VgaTerminal term = VgaTerminal(termTitle, SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = false;
	term.writeXY(79, 24, "0", 7, 0);
	EXPECT_EQ(79, term.getX());
	EXPECT_EQ(24, term.getY());

	term.writeXY(79, 20, "0", 7, 0);
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(21, term.getY());
}

TEST(VgaTerminal, newLine)
{
	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);

	term.gotoXY(10, 10);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(11, term.getY());

	term.writeXY(10, 24, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(24, term.getY());
	
	// check if it is really move on a new line, scrolling the terminal
	auto tc = term.at(10, 23);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

TEST(VgaTerminal, atViewport)
{
	VgaTerminal term = VgaTerminal("atViewported", SDL_WINDOW_HIDDEN, -1, 0);

	//viewport
	term.setViewPort(5, 4, 20, 20);
	term.writeXY(5, 5, "t", 7, 1);
	EXPECT_EQ(6, term.getX());
	EXPECT_EQ(5, term.getY());

	auto tc = term.at(5, 5);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

TEST(VgaTerminal, newLineViewport)
{
	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);

	//viewport
	term.setViewPort(5, 4, 20, 20);
	term.writeXY(5, 18, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	auto tc = term.at(5, 18);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);

	term.clear();
	term.writeXY(5, 19, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	tc = term.at(5, 18);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

TEST(VgaTerminal, newLineNoAutoScroll)
{
	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = false;

	term.gotoXY(10, 10);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(11, term.getY());

	term.writeXY(10, 24, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(24, term.getY());

	// check if it is NOT move on a new line, NO scrolling the terminal
	// So basically doing just a CR on last line when autoscroll is not enabled.
	auto tc = term.at(10, 24);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

TEST(VgaTerminal, newLineNoAutoScrollViewport)
{
	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = false;

	//viewport
	term.setViewPort(5, 4, 20, 20);
	term.writeXY(5, 18, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	auto tc = term.at(5, 18);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);

	term.clear();
	term.writeXY(5, 19, "test", 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	tc = term.at(5, 19);
	EXPECT_EQ('t', tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}

#endif



int main(int argc, char** argv) {
	env = ::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(env);
	return RUN_ALL_TESTS();
}
