aux_source_directory(. SCHEME_SRCS)
add_library(scheme ${SCHEME_SRCS})
target_link_libraries(scheme ds)
