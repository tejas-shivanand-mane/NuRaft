
# === Example replicated calculator ===
add_executable(calc_server
               calculator/calc_server.cxx
               logger.cc
               in_memory_log_store.cxx)

# add_dependencies(calc_server
#                  ${LIBRARY_NAME})
target_link_libraries(calc_server
                      ${LIBRARY_NAME}
                      ${LIBRARIES})


# === Example replicated calculator ===
add_executable(set_server
        set_ops/set_server.cxx
        logger.cc
        in_memory_log_store.cxx)

target_link_libraries(set_server
        ${LIBRARY_NAME}
        ${LIBRARIES})


# === Example replicated calculator ===
add_executable(stack_server
        stack_ops/stack_server.cxx
        logger.cc
        in_memory_log_store.cxx)

target_link_libraries(stack_server
        ${LIBRARY_NAME}
        ${LIBRARIES})


# === Example replicated calculator ===
add_executable(project_server
        project_ops/project_server.cxx
        logger.cc
        in_memory_log_store.cxx)

target_link_libraries(project_server
        ${LIBRARY_NAME}
        ${LIBRARIES})



add_executable(echo_server
               echo/echo_server.cxx
               logger.cc
               in_memory_log_store.cxx)
# add_dependencies(echo_server
#                  ${LIBRARY_NAME})
target_link_libraries(echo_server
                      ${LIBRARY_NAME}
                      ${LIBRARIES})

add_executable(quick_start
               quick_start.cxx
               logger.cc
               in_memory_log_store.cxx)
# add_dependencies(quick_start
#                  ${LIBRARY_NAME})
target_link_libraries(quick_start
                      ${LIBRARY_NAME}
                      ${LIBRARIES})

