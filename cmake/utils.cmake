
# Add strong warnings to target
function(append_warn_flags target)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else ()
       target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif() 
endfunction()

# Add default options to target
function(set_default_opts target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
    )
    append_warn_flags(${target})
endfunction()





