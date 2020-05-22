#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <tui/utils.hpp>
#include "../Environment.hpp"


class  AlignStringTests : public ::testing::TestWithParam<std::tuple<std::string, uint8_t, std::string>> {};
TEST_P(AlignStringTests, alignString)
{
    std::string str = std::get<0>(GetParam());
    uint8_t pad = std::get<1>(GetParam());
    std::string exp = std::get<2>(GetParam());

    EXPECT_EQ(tui::alignString(str, pad, ' '), exp);
}
INSTANTIATE_TEST_SUITE_P(
    utils,
    AlignStringTests,
    ::testing::Values(
        std::make_tuple("", 0, ""),
        std::make_tuple("", 1, " "),
        std::make_tuple("", 2, "  "),
        std::make_tuple("x", 0, "x"),
        std::make_tuple("x", 1, "x "),
        std::make_tuple("x", 2, " x "),
        std::make_tuple("xx", 0, "xx"),
        std::make_tuple("xx", 1, "xx "),
        std::make_tuple("xx", 2, " xx ")
    )
);

class  Align3StringsTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string, uint8_t, uint8_t, std::string>> {};
TEST_P(Align3StringsTests, align3Strings)
{
    std::string left = std::get<0>(GetParam());
    std::string middle = std::get<1>(GetParam());
    std::string right = std::get<2>(GetParam());
    uint8_t expl = std::get<3>(GetParam());
    uint8_t expr = std::get<4>(GetParam());
    std::string exp = std::get<5>(GetParam());
    uint8_t width = exp.size();

    auto [lmark, rmark] = tui::align3Strings(left, middle, right, width);
    uint8_t lpad = lmark - left.size();
    uint8_t mpad = rmark - lmark - middle.size() - 1;
    uint8_t rpad = width - rmark - right.size() - 1;

    std::string res = tui::alignString(left, lpad, ' ')+ "|"
                      + tui::alignString(middle, mpad, ' ') + "|"
                      + tui::alignString(right, rpad, ' ');
    EXPECT_EQ(res.size(), width);
    EXPECT_STRCASEEQ(exp.c_str(), res.c_str());
    EXPECT_EQ(lmark, expl);
    EXPECT_EQ(rmark, expr);
}
INSTANTIATE_TEST_SUITE_P(
    utils,
    Align3StringsTests,
    ::testing::Values(                      //012345678901234567890
        std::make_tuple("a", "b", "c", 1, 3, "a|b|c"),                                                                  // 0
        std::make_tuple("a", "b", "c", 2, 4, "a |b|c"),                                                                 // 1
        std::make_tuple("a", "b", "c", 2, 5, "a |b |c"),                                                                // 2
        std::make_tuple("a", "b", "c", 2, 5, "a |b |c "),                                                               // 3
        std::make_tuple("a", "b", "c", 3, 7, " a | b |c "),                                                             // 4
        std::make_tuple("a", "b", "c", 3, 6, " a |b |c "),                                                              // 5
        std::make_tuple("a", "b", "c", 3, 7, " a | b |c "),                                                             // 6
        std::make_tuple("a", "b", "c", 3, 7, " a | b | c "),                                                            // 7
                                                                    //012345678901234567890123456789012345678901234567890
        std::make_tuple("TITLE", "DESCRIPTION", "VERSION", 07, 21, " TITLE | DESCRIPTION | VERSION "),                  // 8
        std::make_tuple("TITLE", "DESCRIPTION", "VERSION", 12, 31, "   TITLE    |   DESCRIPTION    |   VERSION   ")     // 9
    )
);

class  Align3StringsInvalidArgumentTests : public ::testing::TestWithParam<std::tuple<std::string, std::string, std::string, uint8_t>> {};
TEST_P(Align3StringsInvalidArgumentTests, align3StringsInvalidArgument)
{
    std::string left = std::get<0>(GetParam());
    std::string middle = std::get<1>(GetParam());
    std::string right = std::get<2>(GetParam());
    uint8_t width = std::get<3>(GetParam());

    EXPECT_THROW(tui::align3Strings(left, middle, right, width), std::invalid_argument);
}
INSTANTIATE_TEST_SUITE_P(
    utils,
    Align3StringsInvalidArgumentTests,
    ::testing::Values(
        std::make_tuple("", "", "", 0),
        std::make_tuple("a", "", "", 0),
        std::make_tuple("a", "b", "", 0),
        std::make_tuple("a", "b", "c", 0),
        std::make_tuple("a", "b", "c", 1),
        std::make_tuple("a", "b", "c", 3),
        std::make_tuple("a", "b", "c", 4),
        std::make_tuple("aa", "b", "c", 5)
    )
);


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment());
    return RUN_ALL_TESTS();
}
