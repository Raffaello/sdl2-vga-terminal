#include "Snapshot.hpp"
#include <TuiTerminal.hpp>


TEST(TuiTerminal, Snapshot)
{
	std::string title = ::testing::UnitTest::GetInstance()->current_test_info()->name();
	std::string snapshotFilename = generateSnapshotFilename();
	TuiTerminal tui = TuiTerminal(title, "TUI " + title);

	tui.draw();
	tui.drawSingleBorderDialog({ 0, 0, 22, 5 }, TuiTerminal::defaultCol, TuiTerminal::defaultBgCol, "TUI Dialog", "TUI Dialog Body");
	tui.drawSingleBorderDialog({ 5, 1, 12, 3 }, 14, 1, "TUI On Top", "Shadowing");
	tui.drawDialog({ 20, 5, 20, 10 }, 15, 1, "Header", "Body");
	tui.progressBar(false, true, 22, 10, 15, 50, 100);
	tui.progressBar(true, false, 22, 12, 15, 50, 100);
	tui.render(true);

	snapShotTest(tui.getWindow(), tui.getRenderer(), snapshotFilename);
}


int main(int argc, char** argv)
{
	return snapshotMain(argc, argv);
}
