cmake_minimum_required(VERSION 3.26)
project(CMP501-Pong-Multiplayer LANGUAGES CXX)

if(WIN32)
    # Windows specific configuration
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++ -static")
elseif(UNIX)
    # Linux/macOS specific configuration
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Define Server + Game executable
add_executable(Server "src/Server/Server.cpp" "src/Server/main.cpp" "src/Game/Pong.cpp" "src/Client/Client.cpp"
        src/Snapshot.cpp)
target_link_libraries(Server PRIVATE sfml-graphics sfml-audio sfml-network)
target_include_directories(Server PRIVATE include include/Server include/Game include/Client)
target_compile_features(Server PRIVATE cxx_std_17)

# Define Client + Game executable
add_executable(Client "src/Client/Client.cpp" "src/Client/main.cpp" "src/Game/Pong.cpp" "src/Server/Server.cpp"
        src/Snapshot.cpp)
target_link_libraries(Client PRIVATE sfml-graphics sfml-audio sfml-network)
target_include_directories(Client PRIVATE include include/Client include/Game include/Server)
target_compile_features(Client PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET Client, Server
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:Client>
        VERBATIM)
endif()
