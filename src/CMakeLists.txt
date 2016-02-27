aux_source_directory(. SRC_SRCS)

add_subdirectory(ds)
add_subdirectory(scheme)
add_subdirectory(translate)

add_executable(funny main.c)

target_link_libraries(funny m ds scheme translate)

install(TARGETS funny RUNTIME DESTINATION funny/bin)
