#include "Environment.hpp"
#include "Snapshot.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <VgaTerminal.hpp>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstring>


void vgaTerminalSnapShotTest(VgaTerminal& term, const std::string& snapshotFilename)
{
    // TODO this line smells
    // BODY a common beaviour of a renderer interface has to be considered now
    term.render(true);

    snapShotTest(term.getWindow(), term.getRenderer(), snapshotFilename);
}

TEST(VgaTerminal, Snapshot)
{
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

    vgaTerminalSnapShotTest(term, snapshotFilename);
}

class  CursorShapeTests: public ::testing::TestWithParam<VgaTerminal::CURSOR_MODE> {};
TEST_P(CursorShapeTests, CursorShapeSnapshot)
{
    VgaTerminal::CURSOR_MODE cursorMode = GetParam();
    std::string snapshotFilename = generateSnapshotFilename();
    std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
    VgaTerminal term = VgaTerminal(title, 0, -1, 0);
    term.cursor_mode = cursorMode;
    term.blinkCursor = false;
    term.write(title, 7, 0);

    vgaTerminalSnapShotTest(term, snapshotFilename);
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

int main(int argc, char** argv)
{
    return snapshotMain(argc, argv);
}
