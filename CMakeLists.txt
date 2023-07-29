cmake_minimum_required(VERSION 3.25)
project(cgptchatbot C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

find_package(CURL REQUIRED)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})

include_directories(lib/Unity-2.5.2/src)

add_executable(cgptchatbot src/main.c src/openai.c src/openai.h src/cJSON.c src/cJSON.h src/util.h src/util.c src/appdata.c src/appdata.h src/chat.c src/chat.h)
target_link_libraries(cgptchatbot ${GTK4_LIBRARIES})
target_link_libraries(cgptchatbot CURL::libcurl)

add_executable(openai_test src/openai.c src/openai.h src/cJSON.c src/cJSON.h lib/Unity-2.5.2/src/unity.c lib/Unity-2.5.2/src/unity.h tests/test_openai.c src/util.h src/util.c tests/mock_server.c tests/mock_server.h)
add_executable(appdata_test lib/Unity-2.5.2/src/unity.c lib/Unity-2.5.2/src/unity.h src/cJSON.c src/cJSON.h src/util.h src/util.c src/appdata.c src/appdata.h tests/test_appdata.c)
add_executable(chat_test lib/Unity-2.5.2/src/unity.c lib/Unity-2.5.2/src/unity.h src/util.h src/util.c src/openai.c src/openai.h src/cJSON.c src/cJSON.h src/chat.c src/chat.h tests/test_chat.c)
target_link_libraries(openai_test CURL::libcurl)
target_link_libraries(openai_test microhttpd)
target_link_libraries(appdata_test ${GTK4_LIBRARIES})
enable_testing()
add_test(tests openai_test)
add_test(tests appdata_test)
add_test(tests chat_test)