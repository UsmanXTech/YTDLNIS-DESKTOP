add_executable(YTDLnisTests
    command_line_tests.cpp
)

target_include_directories(YTDLnisTests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_compile_features(YTDLnisTests PRIVATE cxx_std_20)

enable_testing()
add_test(NAME YTDLnis.CommandLine COMMAND YTDLnisTests)
