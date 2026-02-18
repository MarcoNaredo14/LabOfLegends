cmake_minimum_required(VERSION 3.16)
project(Project1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    if(EXISTS "/opt/homebrew/opt/sfml@2")
        list(PREPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/sfml@2")
    endif()
    if(EXISTS "/usr/local/opt/sfml@2")
        list(PREPEND CMAKE_PREFIX_PATH "/usr/local/opt/sfml@2")
    endif()
    list(APPEND CMAKE_PREFIX_PATH
        /opt/homebrew
        /usr/local
    )
endif()

find_package(SFML 2.6 COMPONENTS graphics window system audio CONFIG REQUIRED)

add_executable(Project1
    main.cpp
)

set(PROJECT_LOGIC_FILES
    game.cpp
    player.cpp
    enemy.cpp
    bullet.cpp
    scoreboard.cpp
    lives.cpp
    golditems.cpp
    bgmusic.cpp
)

# These files are included transitively from main.cpp/game.cpp.
# Marking them header-only keeps them visible in CLion without compiling twice.
set_source_files_properties(${PROJECT_LOGIC_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
target_sources(Project1 PRIVATE ${PROJECT_LOGIC_FILES})

target_include_directories(Project1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(TARGET SFML::Graphics)
    target_link_libraries(Project1 PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
    )
else()
    target_link_libraries(Project1 PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
    )
endif()

set(ASSET_DIRS
    menu
    player
    enemy
    items
)

set(ASSET_FILES
    arial.ttf
    background.png
    bullet.png
    wall.png
)

foreach(asset_dir IN LISTS ASSET_DIRS)
    add_custom_command(TARGET Project1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${asset_dir}
            $<TARGET_FILE_DIR:Project1>/${asset_dir}
    )
endforeach()

foreach(asset_file IN LISTS ASSET_FILES)
    add_custom_command(TARGET Project1 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${asset_file}
            $<TARGET_FILE_DIR:Project1>/${asset_file}
    )
endforeach()
