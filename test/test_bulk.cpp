#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "command_processor.h"
#include "logger.h"
#include "project_path.h"
#include <string>

using namespace std::string_literals;

struct TestParams {
    int num_commands_in_bulk;
    std::string in_file;
    std::string out_file;
    bool use_separate_thread_for_cout = false;
};

class AssignmentTestFromFile : public testing::TestWithParam<TestParams> {
};

TEST_P(AssignmentTestFromFile, AssignmentExample) {
    auto num_commands_in_bulk = GetParam().num_commands_in_bulk;
    auto in_file = GetParam().in_file;
    auto out_file = GetParam().out_file;
    std::fstream test_input(in_file);
    std::fstream test_output_etalon_file(out_file);

    std::stringstream test_output_etalon_stream;
    test_output_etalon_stream << test_output_etalon_file.rdbuf();

    CommandProcessor processor(num_commands_in_bulk);
    if (GetParam().use_separate_thread_for_cout)
        Logger::get_logger().reserve_thread_for_stdout();

    testing::internal::CaptureStdout();
    processor.process_commands(test_input);
    if (GetParam().use_separate_thread_for_cout)
        Logger::get_logger().suspend_work();
    std::string output = testing::internal::GetCapturedStdout();
    if (GetParam().use_separate_thread_for_cout)
        Logger::get_logger().resume_work();

    ASSERT_EQ(output, test_output_etalon_stream.str());
}

INSTANTIATE_TEST_CASE_P(MyGroup, AssignmentTestFromFile, ::testing::Values(
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test1.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test1.out.txt"s},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test2.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test2.out.txt"s},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test3.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test3.out.txt"s},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test4.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test4.out.txt"s},
        TestParams{2,
                   PROJECT_SOURCE_DIR + "/test/data/test5.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test5.out.txt"s}, // output all before first bracket
//         tests for separated thread for cout
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test1.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test1.out.txt"s,
                   true},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test2.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test2.out.txt"s,
                   true},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test3.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test3.out.txt"s,
                   true},
        TestParams{3,
                   PROJECT_SOURCE_DIR + "/test/data/test4.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test4.out.txt"s,
                   true},
        TestParams{2,
                   PROJECT_SOURCE_DIR + "/test/data/test5.in.txt"s,
                   PROJECT_SOURCE_DIR + "/test/data/test5.out.txt"s,
                   true} // output all before first bracket
));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
