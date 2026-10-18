# lmon_enable_sanitizers(<target>)
#
# Adds the sanitizers selected by LMON_ENABLE_ASAN / LMON_ENABLE_UBSAN to a target.

function(lmon_enable_sanitizers target)
    set(sanitizers "")

    if(LMON_ENABLE_ASAN)
        list(APPEND sanitizers address)
    endif()

    if(LMON_ENABLE_UBSAN)
        list(APPEND sanitizers undefined)
    endif()

    if(NOT sanitizers)
        return()
    endif()

    list(JOIN sanitizers "," sanitizer_list)
    target_compile_options(${target} PRIVATE
        -fsanitize=${sanitizer_list}
        -fno-omit-frame-pointer
        -fno-sanitize-recover=all)
    target_link_options(${target} PRIVATE -fsanitize=${sanitizer_list})
endfunction()
