add_executable(YTDLnisTests
    command_line_tests.cpp
)

target_include_directories(YTDLnisTests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(YTDLnisTests PRIVATE cxx_std_20)

enable_testing()
add_test(NAME YTDLnis.CommandLine COMMAND YTDLnisTests)

add_executable(YTDLnisProgressParserTests
    progress_parser_tests.cpp
    ${PROJECT_SOURCE_DIR}/src/ytdl/ProgressParser.cpp
)
target_include_directories(YTDLnisProgressParserTests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)
target_compile_features(YTDLnisProgressParserTests PRIVATE cxx_std_20)
add_test(NAME YTDLnis.ProgressParser COMMAND YTDLnisProgressParserTests)
