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

void cmpViewportCheck(const SDL_Rect& vp, const SDL_Rect& exp)
{
	EXPECT_EQ(vp.x, exp.x);
	EXPECT_EQ(vp.y, exp.y);
	EXPECT_EQ(vp.w, exp.w);
	EXPECT_EQ(vp.h, exp.h);
}

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
	cmpViewportCheck(r, e);
	EXPECT_TRUE(term.setViewPort(r));
	e = term.getViewport();
	cmpViewportCheck(r, e);
	
	testing::internal::CaptureStderr();
	EXPECT_FALSE(term.setViewPort(0, 0, 1, 0));
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	cmpViewportCheck(r, e);
	
	testing::internal::CaptureStderr();
	EXPECT_FALSE(term.setViewPort(0, 0, 0, 1));
	output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, StartsWith("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));

	e = term.getViewport();
	cmpViewportCheck(r, e);
}

TEST(VgaTerminal, moveCursorCircle)
{
	VgaTerminal term = VgaTerminal("MoveCursor", SDL_WINDOW_HIDDEN, -1, 0);
	
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

class  SetViewportParameterTests : public ::testing::TestWithParam<std::tuple<int, int, int, int, bool, int, int>> {};
TEST_P(SetViewportParameterTests, SetVieport)
{
	int vx = std::get<0>(GetParam());
	int vy = std::get<1>(GetParam());
	int vw = std::get<2>(GetParam());
	int vh = std::get<3>(GetParam());
	bool autoScroll = std::get<4>(GetParam());
	int x = std::get<5>(GetParam());
	int y = std::get<6>(GetParam());
		
	VgaTerminal term = VgaTerminal("SetViewport Test", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = autoScroll;

	EXPECT_TRUE(term.setViewPort(vx, vy, vw, vh));
	auto e = term.getViewport();
	EXPECT_EQ(vx, e.x);
	EXPECT_EQ(vy, e.y);
	EXPECT_EQ(vw, e.w);
	EXPECT_EQ(vh, e.h);
	term.writeXY(x, y, "X", 15, 1);
	auto tc = term.at(x, y);
	EXPECT_EQ('X', tc.c);
	EXPECT_EQ(15, tc.col);
	EXPECT_EQ(1, tc.bgCol);
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	SetViewportParameterTests,
	::testing::Values(
		std::make_tuple(0, 0, 1, 1, false, 0, 0),    // SetViewport1
		std::make_tuple(10, 10, 10, 10, true, 0, 0), // SetViewport2
		std::make_tuple(5, 4, 20, 20, true, 5, 5)    // atViewport
	)
);

class  MoveCursorBorderCW : public ::testing::TestWithParam<std::tuple<int, int, int, int>> {};
TEST_P(MoveCursorBorderCW, moveCursorBoderCW)
{
	SDL_Rect vp;
	vp.x = std::get<0>(GetParam());
	vp.y = std::get<1>(GetParam());
	vp.w = std::get<2>(GetParam());
	vp.h = std::get<3>(GetParam());
	ASSERT_GT(vp.h, 3);
	ASSERT_GT(vp.w, 2);

	VgaTerminal term = VgaTerminal("SetViewport Test", SDL_WINDOW_HIDDEN, -1, 0);
	ASSERT_TRUE(term.setViewPort(vp));
	
	
	term.gotoXY(vp.w - 1, vp.h - 2);
	term.moveCursorRight();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(vp.h - 1, term.getY());
	term.moveCursorDown();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(vp.h - 1, term.getY());
	term.moveCursorLeft();
	EXPECT_EQ(vp.w - 1, term.getX());
	EXPECT_EQ(vp.h - 2, term.getY());
	term.moveCursorUp();
	EXPECT_EQ(vp.w - 1, term.getX());
	EXPECT_EQ(vp.h - 3, term.getY());
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	MoveCursorBorderCW,
	::testing::Values(
		std::make_tuple(0, 0, 80, 25),
		std::make_tuple(10, 10, 10, 10)
	)
);

class  NewLineParameterTests : public ::testing::TestWithParam<std::tuple<int, int, bool>> {};
TEST_P(NewLineParameterTests, newLine)
{
	int x = std::get<0>(GetParam());
	int y = std::get<1>(GetParam());
	bool autoScroll = std::get<2>(GetParam());

	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = autoScroll;
	auto vp = term.getViewport();
	char c = 'X';
	uint8_t col = 7;
	uint8_t bgCol = 1;

	ASSERT_GE(vp.h, 2);

	term.gotoXY(x, y);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(x+1, term.getY());

	term.writeXY(x, vp.h - 1, std::string(1, c), 7, 1);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(vp.h - 1, term.getY());

	// check if it is really move on a new line, scrolling the terminal
	auto tc = term.at(x, vp.h - 1 - static_cast<uint8_t>(autoScroll));
	EXPECT_EQ(c, tc.c);
	EXPECT_EQ(col, tc.col);
	EXPECT_EQ(bgCol, tc.bgCol);
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	NewLineParameterTests,
	::testing::Values(
		std::make_tuple(10, 10, true),    // NewLine
		std::make_tuple(10, 10, false) // no autoscroll
	)
);

class  NewLineViewPortTests : public ::testing::TestWithParam<bool> {};
TEST_P(NewLineViewPortTests, newLineViewport)
{
	bool autoScroll = GetParam();
	VgaTerminal term = VgaTerminal("newLine", SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = autoScroll;
	std::string str = "X";
	uint8_t col = 7;
	uint8_t bgCol = 1;

	//viewport
	term.setViewPort(5, 4, 20, 20);
	term.writeXY(5, 18, str, col, bgCol);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	auto tc = term.at(5, 18);
	EXPECT_EQ(str[0], tc.c);
	EXPECT_EQ(7, tc.col);
	EXPECT_EQ(1, tc.bgCol);

	term.clear();
	term.writeXY(5, 19, str, col, bgCol);
	term.newLine();
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(19, term.getY());

	tc = term.at(5, 19 - static_cast<uint8_t>(autoScroll));
	EXPECT_EQ(str[0], tc.c);
	EXPECT_EQ(col, tc.col);
	EXPECT_EQ(bgCol, tc.bgCol);
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	NewLineViewPortTests,
	::testing::Values(
		true,
		false
	)
);
#endif


int main(int argc, char** argv) {
	env = ::testing::AddGlobalTestEnvironment(new Environment());
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(env);
	return RUN_ALL_TESTS();
}
