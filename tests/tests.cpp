#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

// Platform configuration
#ifdef _WIN32
    #define CLUB_EXE "Club.exe"
    #define OUTPUT_FILE "output.txt"
    #define CLEANUP_CMD "del " OUTPUT_FILE
#else
    #define CLUB_EXE "./Club"
    #define OUTPUT_FILE "output.txt"
    #define CLEANUP_CMD "rm -f " OUTPUT_FILE
#endif

class EndToEndTest : public ::testing::TestWithParam<int> {
protected:
    std::string test_data_dir;

    EndToEndTest() {
        test_data_dir = (fs::current_path() / "tests" / "data").string() + "/";
    }

    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file) return "";
        return {std::istreambuf_iterator<char>(file), {}};
    }

    std::string run_program(const std::string& input_path) {
        const std::string abs_input = fs::absolute(input_path).string();
        const std::string command =
            std::string(CLUB_EXE) + " \"" + abs_input + "\" > " + OUTPUT_FILE;

        std::system(command.c_str());
        return read_file(OUTPUT_FILE);
    }

    void compare_outputs(const std::string& actual, const std::string& expected) {
        std::istringstream a(actual), e(expected);
        std::string aline, eline;
        int line = 1;
        std::ostringstream diff_output;

        while (true) {
            bool a_has_line = !!std::getline(a, aline);
            bool e_has_line = !!std::getline(e, eline);

            if (!a_has_line && !e_has_line) break;

            diff_output << "Line " << line << ":\n";
            if (a_has_line) diff_output << "  Actual:   " << aline << "\n";
            else diff_output << "  Actual:   <END OF OUTPUT>\n";

            if (e_has_line) diff_output << "  Expected: " << eline << "\n";
            else diff_output << "  Expected: <END OF OUTPUT>\n";

            if (!a_has_line || !e_has_line || aline != eline) {
                FAIL() << "Output mismatch:\n" << diff_output.str();
            }

            line++;
        }
    }
};

TEST_P(EndToEndTest, AllTestCases) {
    const int test_num = GetParam();
    const std::string base = "test" + std::to_string(test_num);
    const std::string input = test_data_dir + base + ".in";
    const std::string expected = test_data_dir + base + ".out";

    std::cout << "Running test " << test_num << "\n";
    std::cout << "Input file: " << input << "\n";
    std::cout << "Expected file: " << expected << "\n";

    if (!fs::exists(input)) {
        FAIL() << "Input file missing: " << input;
    }
    if (!fs::exists(expected)) {
        FAIL() << "Expected file missing: " << expected;
    }

    std::cout << "Running command: " << CLUB_EXE << " " << input << " > " << OUTPUT_FILE << "\n";

    const std::string actual = run_program(input);
    const std::string expected_content = read_file(expected);

    std::cout << "----- ACTUAL OUTPUT -----\n";
    std::cout << actual << "\n";
    std::cout << "----- EXPECTED OUTPUT -----\n";
    std::cout << expected_content << "\n";
    std::cout << "--------------------------\n";

    compare_outputs(actual, expected_content);
    std::system(CLEANUP_CMD);
}

INSTANTIATE_TEST_SUITE_P(
    ClubTests,
    EndToEndTest,
    ::testing::Range(1, 10),
    [](const auto& info) {
        return "Test" + std::to_string(info.param);
    }
);