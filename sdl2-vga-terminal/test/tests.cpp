#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>
#include <cmath>
#include "Environment.hpp"
#include <cstring>


void cmpViewportCheck(const SDL_Rect& vp, const SDL_Rect& exp)
{
	EXPECT_EQ(vp.x, exp.x);
	EXPECT_EQ(vp.y, exp.y);
	EXPECT_EQ(vp.w, exp.w);
	EXPECT_EQ(vp.h, exp.h);
}

// expect around ~1% tolerance on the given value to be matched
void cmpTicks(const uint32_t start, const uint32_t end, const uint16_t value)
{
	double te = static_cast<double>(value) * 0.01;
	double t = abs(static_cast<double>(end - start) - value) - te;
	EXPECT_LE(t, 1);
}

TEST(VgaTerminal, checkVersion)
{
	VgaTerminal t("", 0, -1, 0);
	ASSERT_STRCASEEQ("0.2.2", t.getVersion().c_str());
}

TEST(VgaTerminal, CannotInit)
{
	// NOTE: using the env ptr, generate a segFault at the end of the test.. weird.
	//       so created 2 static wrapper.
	Environment::tearDown();
	ASSERT_THROW(VgaTerminal term = VgaTerminal("", 0, -1, 0), std::runtime_error);
	Environment::setUp();
}

TEST(VgaTerminal, TimerNotInitedWarning)
{
	Environment::tearDown();
	ASSERT_EQ(0, SDL_Init(SDL_INIT_VIDEO));
	SDL_LogSetPriority(SDL_LOG_CATEGORY_SYSTEM, SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);
	{
		testing::internal::CaptureStderr();
		std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
		VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
		std::string output = testing::internal::GetCapturedStderr();
		EXPECT_THAT(output, testing::EndsWith("TIMER or EVENTS not inited.\n"));
		EXPECT_THAT(output, testing::HasSubstr("WARN:"));
	}
	Environment::tearDown();
	Environment::setUp();
}

TEST(VgaTerminal, HelloWorldWindow) {
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	
	ASSERT_EQ(SDL_GetWindowTitle(term.getWindow()), title);
}

TEST(VgaTerminal, HelloWorldText) {
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);

	term.write(title, 7, 0);
	uint8_t termTitleLength = static_cast<uint8_t>(title.size());
	EXPECT_EQ(VgaTerminal::position_t(termTitleLength, 0), term.getXY());
	EXPECT_EQ(VgaTerminal::position_t(termTitleLength, 0), VgaTerminal::position_t(term.getX(), term.getY()));
	EXPECT_EQ(title[6], term.at(6, 0).c);
}

TEST(VgaTerminal, ScrollDown) {
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);

	term.writeXY(term.getMode().tw - 1, term.getMode().th - 1, title, 7, 1);
	uint8_t titleLength = static_cast<uint8_t>(title.size());

	EXPECT_EQ(VgaTerminal::position_t(titleLength - 1, term.getMode().th - 1), term.getXY());
	EXPECT_EQ(title[0], term.at(term.getMode().tw - 1, term.getMode().th - 2).c);
	EXPECT_EQ(title[6], term.at(5, term.getMode().th - 1).c);
}

TEST(VgaTerminal, moveCursorClockWise)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	
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
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	term.autoScroll = false;
	term.writeXY(79, 24, "0", 7, 0);
	EXPECT_EQ(79, term.getX());
	EXPECT_EQ(24, term.getY());

	term.writeXY(79, 20, "0", 7, 0);
	EXPECT_EQ(0, term.getX());
	EXPECT_EQ(21, term.getY());
}

TEST(VgaTerminal, clear)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	const char c = 'X';
	const uint8_t col = 7;
	const uint8_t bgCol = 1;

	term.write(c, col, bgCol);
	auto tc = term.at(0, 0);
	EXPECT_EQ(c, tc.c);
	EXPECT_EQ(col, tc.col);
	EXPECT_EQ(bgCol, tc.bgCol);
	
	term.clear();
	tc = term.at(0, 0);
	EXPECT_EQ(0, tc.c);
	EXPECT_EQ(0, tc.col);
	EXPECT_EQ(0, tc.bgCol);
}

TEST(VgaTerminal, clearViewport)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	const char c = 'X';
	const char c2 = 'Y';
	const uint8_t col = 7;
	const uint8_t bgCol = 1;
	
	term.gotoXY(9, 10);
	term.write(c2, col, bgCol);

	ASSERT_TRUE(term.setViewPort(10, 10, 10, 10));

	term.write(c, col, bgCol);
	auto tc = term.at(0, 0);
	EXPECT_EQ(c, tc.c);
	EXPECT_EQ(col, tc.col);
	EXPECT_EQ(bgCol, tc.bgCol);

	term.clear();
	tc = term.at(0, 0);
	EXPECT_EQ(0, tc.c);
	EXPECT_EQ(0, tc.col);
	EXPECT_EQ(0, tc.bgCol);

	term.resetViewport();
	tc = term.at(9, 10);
	EXPECT_EQ(c2, tc.c);
	EXPECT_EQ(col, tc.col);
	EXPECT_EQ(bgCol, tc.bgCol);
}

TEST(VgaTerminal, resetViewport)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	auto vp = term.getViewport();
	SDL_Rect vp2 = { 10, 10 ,10, 10 };
	ASSERT_TRUE(term.setViewPort(vp2));
	cmpViewportCheck(vp2, term.getViewport());
	term.resetViewport();
	cmpViewportCheck(vp, term.getViewport());
}

TEST(VgaTerminal, Idle)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	term.cursor_time = 1000;
	EXPECT_TRUE(term.isIdle());
	term.write("X", 7, 1);
	EXPECT_FALSE(term.isIdle());
}

class  CursorBlinkingTests: public ::testing::TestWithParam<uint16_t> {};
TEST_P(CursorBlinkingTests, cursorBlinking)
{
	uint16_t cursorTime = GetParam();
	SDL_Event e;
	std::memset(&e, 0, sizeof(SDL_Event));

	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	VgaTerminal term = VgaTerminal(title, SDL_WINDOW_HIDDEN, -1, 0);
	
	uint32_t init = SDL_GetTicks();
	uint16_t old_time = term.cursor_time;
	term.cursor_time = cursorTime;
	ASSERT_EQ(1, SDL_WaitEventTimeout(&e, 1000));
	uint32_t start = SDL_GetTicks();
	ASSERT_EQ(1, SDL_WaitEventTimeout(&e, cursorTime * 2));
	uint32_t end = SDL_GetTicks();

	EXPECT_EQ(e.type, SDL_USEREVENT);
	EXPECT_EQ(e.window.windowID, term.getWindowId());
	EXPECT_EQ(e.user.code, 0);
	EXPECT_TRUE(NULL == e.user.data1);
	EXPECT_TRUE(NULL == e.user.data2);
	EXPECT_EQ(e.user.type, SDL_USEREVENT); // ???
	EXPECT_EQ(e.user.windowID, term.getWindowId());
	// give a tollerance of 1% 
	cmpTicks(start, end, cursorTime);
	cmpTicks(init, start, old_time);
}

class  SetViewportParameterTests: public ::testing::TestWithParam<std::tuple<int, int, int, int, bool, int, int>> {};
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
		std::make_tuple<int, int, int, int, bool, int, int>(0, 0, 1, 1, false, 0, 0),    // SetViewport1
		std::make_tuple<int, int, int, int, bool, int, int>(10, 10, 10, 10, true, 0, 0), // SetViewport2
		std::make_tuple<int, int, int, int, bool, int, int>(5, 4, 20, 20, true, 5, 5)    // atViewport
	)
);

class SetViewportNullErrTests: public ::testing::TestWithParam<std::tuple<int, int, int, int>> {};
TEST_P(SetViewportNullErrTests, setViewportNullError)
{
	using ::testing::HasSubstr;
	using ::testing::EndsWith;

	SDL_Rect vp;
	vp.x = std::get<0>(GetParam());
	vp.y = std::get<1>(GetParam());
	vp.w = std::get<2>(GetParam());
	vp.h = std::get<3>(GetParam());

	VgaTerminal term = VgaTerminal("setViewportNullErr", SDL_WINDOW_HIDDEN, -1, 0);

	testing::internal::CaptureStderr();
	EXPECT_FALSE(term.setViewPort(vp));
	std::string output = testing::internal::GetCapturedStderr();
	EXPECT_THAT(output, HasSubstr("WARN: ["));
	EXPECT_THAT(output, EndsWith("VgaTerminal] setViewPort: viewport too small.\n"));
}
INSTANTIATE_TEST_SUITE_P(
	VgaTerminal,
	SetViewportNullErrTests,
	::testing::Values(
		std::make_tuple<int, int, int, int>(0, 0, 0, 0),
		std::make_tuple<int, int, int, int>(0, 0, 1, 0),
		std::make_tuple<int, int, int, int>(0, 0, 1, 0)
	)
);

class  MoveCursorBorderCW: public ::testing::TestWithParam<std::tuple<int, int, int, int>> {};
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

class  NewLineParameterTests: public ::testing::TestWithParam<std::tuple<int, int, bool>> {};
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
		std::make_tuple(10, 10, true), // NewLine
		std::make_tuple(10, 10, false) // no autoscroll
	)
);

class  NewLineViewPortTests: public ::testing::TestWithParam<bool> {};
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


int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::AddGlobalTestEnvironment(new Environment());
	return RUN_ALL_TESTS();
}
