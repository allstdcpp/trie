cmake_minimum_required(VERSION 3.26)
project(trie)

set(CMAKE_CXX_STANDARD 20)

add_executable(trie main.cpp)
target_link_libraries(trie PRIVATE -fsanitize=address)
target_compile_options(trie PRIVATE -fsanitize=address)
