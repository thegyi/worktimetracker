cmake_minimum_required(VERSION 3.16)
project(WorkTimeTracker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 or Qt5
find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets)
endif()

# Set Qt variable for either version
if (Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    set(QT_LIBRARIES Qt6::Core Qt6::Gui Qt6::Widgets)
else()
    set(QT_VERSION_MAJOR 5)
    set(QT_LIBRARIES Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/worktimetracker.cpp
)

set(HEADERS
    include/worktimetracker.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${QT_LIBRARIES})

# Installation
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
