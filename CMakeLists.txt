cmake_minimum_required(VERSION 3.27)
project(Redemption_through_Rebellion)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_SHARED_LIBS FALSE)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY  https://github.com/fmtlib/fmt
        GIT_TAG         10.2.1
)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY  https://github.com/SFML/SFML
        GIT_TAG         2.6.1
)

FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(SFML)

add_executable(Redemption_through_Rebellion src/main.cpp
        README.md
        src/inventory/inv.cpp
        src/inventory/inv.hpp
        src/level/lev.cpp
        src/level/lev.hpp
        src/game/game.cpp
        src/game/game.hpp
        src/menu/menu.cpp
        src/menu/menu.hpp
        src/animation/anim.cpp
        src/animation/anim.hpp
        src/level/door.cpp
        src/level/door.hpp
)

target_link_libraries(Redemption_through_Rebellion PRIVATE fmt::fmt)
target_link_libraries(Redemption_through_Rebellion PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)