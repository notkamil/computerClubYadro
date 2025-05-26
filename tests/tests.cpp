#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

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

struct TestCasePaths {
    std::string input_path;
    std::string output_path;
    std::string test_name;
};

std::vector<TestCasePaths> discover_tests(const std::string& test_data_dir) {
    std::vector<TestCasePaths> test_cases;

    if (!fs::exists(test_data_dir)) {
        std::cerr << "Test data directory not found: " << test_data_dir << std::endl;
        return test_cases;
    }

    for (const auto& entry : fs::recursive_directory_iterator(test_data_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".in") {
            fs::path input_path = entry.path();
            fs::path output_path = input_path;
            output_path.replace_extension(".out");

            if (!fs::exists(output_path)) {
                std::cerr << "Output file not found for input: " << input_path << std::endl;
                continue;
            }

            fs::path relative_path = fs::relative(input_path, test_data_dir);
            relative_path = relative_path.replace_extension(""); // Remove .in extension

            std::string test_name = relative_path.generic_string(); // Use forward slashes
            std::replace(test_name.begin(), test_name.end(), '/', '_');
            std::replace(test_name.begin(), test_name.end(), '\\', '_');

            test_cases.push_back({
                input_path.string(),
                output_path.string(),
                test_name
            });
        }
    }

    return test_cases;
}

const std::vector<TestCasePaths>& get_test_cases() {
    static std::vector<TestCasePaths> cases = []() {
        std::string test_data_dir = (fs::current_path() / "tests" / "data").string();
        return discover_tests(test_data_dir);
    }();
    return cases;
}

class EndToEndTest : public ::testing::TestWithParam<TestCasePaths> {
protected:
    std::string read_file(const std::string& path) {
        std::ifstream file(path);
        if (!file) return "";
        return {std::istreambuf_iterator<char>(file), {}};
    }

    std::string run_program(const std::string& input_path) {
        const std::string command =
            std::string(CLUB_EXE) + " \"" + input_path + "\" > " OUTPUT_FILE;

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
    const auto& test_case = GetParam();

    std::cout << "Running test: " << test_case.test_name << "\n";
    std::cout << "Input file: " << test_case.input_path << "\n";
    std::cout << "Expected file: " << test_case.output_path << "\n";

    if (!fs::exists(test_case.input_path)) {
        FAIL() << "Input file missing: " << test_case.input_path;
    }
    if (!fs::exists(test_case.output_path)) {
        FAIL() << "Expected file missing: " << test_case.output_path;
    }

    const std::string actual = run_program(test_case.input_path);
    const std::string expected_content = read_file(test_case.output_path);

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
    ::testing::ValuesIn(get_test_cases()),
    [](const ::testing::TestParamInfo<TestCasePaths>& info) {
        return info.param.test_name;
    }
);