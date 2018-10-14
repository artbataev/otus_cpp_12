#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "command_processor.h"
#include "logger.h"
#include "project_path.h"
#include "async.h"

using namespace std::string_literals;

struct TestParams {
    int num_commands_in_bulk;
    std::string in_file;
    std::string out_file;
};

class AssignmentTestFromFile : public testing::TestWithParam<TestParams> {
};

TEST_P(AssignmentTestFromFile, AssignmentExample) {
    std::fstream test_input(GetParam().in_file);
    std::fstream test_output_etalon_file(GetParam().out_file);

    std::stringstream test_output_etalon_stream;
    test_output_etalon_stream << test_output_etalon_file.rdbuf();

    CommandProcessor processor(GetParam().num_commands_in_bulk);

    testing::internal::CaptureStdout();
    processor.process_commands(test_input);
    std::string output = testing::internal::GetCapturedStdout();

    ASSERT_EQ(output, test_output_etalon_stream.str());
}

TEST_P(AssignmentTestFromFile, AssignmentExampleSeparateCoutThread) {
    std::fstream test_input(GetParam().in_file);
    std::fstream test_output_etalon_file(GetParam().out_file);

    std::stringstream test_output_etalon_stream;
    test_output_etalon_stream << test_output_etalon_file.rdbuf();

    CommandProcessor processor(GetParam().num_commands_in_bulk);
    Logger::get_logger().reserve_thread_for_stdout();

    testing::internal::CaptureStdout();
    processor.process_commands(test_input);

    Logger::get_logger().suspend_work();
    std::string output = testing::internal::GetCapturedStdout();
    Logger::get_logger().resume_work();

    ASSERT_EQ(output, test_output_etalon_stream.str());
}

TEST(AssignmentTestAsync, AssignmentTestAsync_1) {
    auto h = async::connect(5);
    auto h2 = async::connect(5);

    testing::internal::CaptureStdout();
    async::receive(h, "1", 1);
    async::receive(h2, "1\n", 2);
    async::receive(h, "\n2\n3\n4\n5\n6\n{\na\n", 15);
    async::receive(h, "b\nc\nd\n}\n89\n", 11);
    async::disconnect(h);
    async::disconnect(h2);

    Logger::get_logger().suspend_work();
    async::stop_all_tasks();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "bulk: 1, 2, 3, 4, 5\nbulk: 6\nbulk: a, b, c, d\n");

    Logger::get_logger().resume_work();
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
                   PROJECT_SOURCE_DIR + "/test/data/test5.out.txt"s} // output all before first bracket
));

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
