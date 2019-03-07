add_executable(su2s1o su2s1o.cpp)
add_executable(s1o2su s1o2su.cpp)
add_executable(s1o2su_q s1o2su_q.cpp)

target_link_libraries (su2s1o dl ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (s1o2su dl ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (s1o2su_q dl ${CMAKE_THREAD_LIBS_INIT})
