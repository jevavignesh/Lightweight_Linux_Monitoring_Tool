# lmon_set_warnings(<target>)
#
# Applies the project's warning flags to one of our own targets.
# Third-party code is never passed through this, so its warnings don't break the build.

function(lmon_set_warnings target)
    set(warnings
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wold-style-cast
        -Wnon-virtual-dtor
        -Woverloaded-virtual
        -Wcast-align
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Wunused)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND warnings
            -Wmisleading-indentation
            -Wduplicated-cond
            -Wduplicated-branches
            -Wlogical-op
            -Wuseless-cast)
    endif()

    if(LMON_WARNINGS_AS_ERRORS)
        list(APPEND warnings -Werror)
    endif()

    target_compile_options(${target} PRIVATE ${warnings})
endfunction()
