#Runs the program with the lines of an input file and checks what it prints:
#the expected text on stdout, byte for byte, nothing on stderr and exit code 0.
#Usage: cmake -DPROGRAM=<executable> -DPROGRAM_INPUT=<file> -DEXPECTED_OUTPUT=<file> -P CheckProgramOutput.cmake

execute_process(COMMAND ${PROGRAM}
        INPUT_FILE ${PROGRAM_INPUT}
        OUTPUT_VARIABLE actualOutput
        ERROR_VARIABLE actualErrors
        RESULT_VARIABLE exitCode)
file(READ ${EXPECTED_OUTPUT} expectedOutput)

if (NOT exitCode STREQUAL "0")
    message(FATAL_ERROR "The program ended with ${exitCode} instead of 0")
endif ()

if (NOT actualErrors STREQUAL "")
    message(FATAL_ERROR "The program wrote on stderr:\n${actualErrors}")
endif ()

if (NOT actualOutput STREQUAL expectedOutput)
    message(FATAL_ERROR "The output differs from ${EXPECTED_OUTPUT}; the program printed:\n${actualOutput}")
endif ()
