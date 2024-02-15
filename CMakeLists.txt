cmake_minimum_required(VERSION 3.25)
project(play_with_reactor)

set(CMAKE_CXX_STANDARD 11)


include_directories(/usr/local/mysql-8.3.0-macos14-arm64/include)
link_directories(/usr/local/mysql-connector-c++/lib64) # 链接库目录

add_executable(play_with_reactor main.cpp Connection.h CommonConnectionPool.h Connection.cpp)

target_link_libraries(play_with_reactor -lmysqlcppconn-static -lssl -lcrypto) # 要链接的库文件
