cmake_minimum_required(VERSION 3.8)

# Project
project(darkstone_ps1_tweaks)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Widgets)

# mkpsxiso / dumpsxiso
add_subdirectory(${PROJECT_SOURCE_DIR}/dep/mkpsxiso)

# Exe
set(COMMON_DIR ${PROJECT_SOURCE_DIR}/src/Common)
set(BACKEND_DIR ${PROJECT_SOURCE_DIR}/src/Backend)
set(FRONTENDQT_DIR ${PROJECT_SOURCE_DIR}/src/FrontendQt)

set(SOURCES_COMMON
    ${COMMON_DIR}/Buffer.hpp
    ${COMMON_DIR}/DstException.hpp
    ${COMMON_DIR}/Json.hpp
    ${COMMON_DIR}/RawFile.cpp
    ${COMMON_DIR}/RawFile.hpp
    ${COMMON_DIR}/Software.hpp
    ${COMMON_DIR}/TemplateTypes.hpp
    ${COMMON_DIR}/Types.hpp
    ${COMMON_DIR}/Utility.cpp
    ${COMMON_DIR}/Utility.hpp)

set(SOURCES_BACKEND
    ${BACKEND_DIR}/Game.cpp
    ${BACKEND_DIR}/Game.hpp
    ${BACKEND_DIR}/Mips.hpp
    ${BACKEND_DIR}/Offset.cpp
    ${BACKEND_DIR}/Offset.hpp
    ${BACKEND_DIR}/Path.cpp
    ${BACKEND_DIR}/Path.hpp
    ${BACKEND_DIR}/Tweaks.cpp
    ${BACKEND_DIR}/Tweaks.hpp
    ${BACKEND_DIR}/Version.cpp
    ${BACKEND_DIR}/Version.hpp)

set(SOURCES_FRONTENDQT
    ${FRONTENDQT_DIR}/AboutDialog.cpp
    ${FRONTENDQT_DIR}/AboutDialog.hpp
    ${FRONTENDQT_DIR}/AboutDialog.ui
    ${FRONTENDQT_DIR}/ExtractGameDialog.cpp
    ${FRONTENDQT_DIR}/ExtractGameDialog.hpp
    ${FRONTENDQT_DIR}/ExtractGameDialog.ui
    ${FRONTENDQT_DIR}/Main.cpp
    ${FRONTENDQT_DIR}/MainWindow.cpp
    ${FRONTENDQT_DIR}/MainWindow.hpp
    ${FRONTENDQT_DIR}/MainWindow.ui
    ${FRONTENDQT_DIR}/QtUtility.cpp
    ${FRONTENDQT_DIR}/QtUtility.hpp
    ${FRONTENDQT_DIR}/SaveGameDialog.cpp
    ${FRONTENDQT_DIR}/SaveGameDialog.hpp
    ${FRONTENDQT_DIR}/SaveGameDialog.ui
    ${FRONTENDQT_DIR}/TopInfoWidget.cpp
    ${FRONTENDQT_DIR}/TopInfoWidget.hpp
    ${FRONTENDQT_DIR}/TopInfoWidget.ui
    ${FRONTENDQT_DIR}/TweaksWidget.cpp
    ${FRONTENDQT_DIR}/TweaksWidget.hpp
    ${FRONTENDQT_DIR}/TweaksWidget.ui
    ${FRONTENDQT_DIR}/TweaksWidgetSettings.cpp
    ${FRONTENDQT_DIR}/TweaksWidgetSettings.hpp)

set(SOURCES_RESOURCEQT
    ${PROJECT_SOURCE_DIR}/src/FrontendQt/Resource/resources.qrc)

if (WIN32)
    enable_language("RC")
    set(WIN32_RESOURCE ${FRONTENDQT_DIR}/Resource/DarkstonePS1Tweaks.rc)
    add_executable(darkstone_ps1_tweaks WIN32 ${WIN32_RESOURCE})
else()
    add_executable(darkstone_ps1_tweaks)
endif()

target_sources(darkstone_ps1_tweaks PRIVATE 
    ${SOURCES_FRONTENDQT}
    ${SOURCES_COMMON}
    ${SOURCES_BACKEND}
    ${SOURCES_RESOURCEQT})

source_group(Common FILES ${SOURCES_COMMON})
source_group(Backend FILES ${SOURCES_BACKEND})
source_group(FrontendQt FILES ${SOURCES_FRONTENDQT})
source_group(FrontendQt/Resource FILES ${SOURCES_RESOURCEQT})

target_include_directories(darkstone_ps1_tweaks PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/dep/json/include
    ${PROJECT_SOURCE_DIR}/dep/mkpsxiso/src)

target_compile_definitions(darkstone_ps1_tweaks PRIVATE
    $<$<CONFIG:Debug>:DST_DEBUG>)

target_link_libraries(darkstone_ps1_tweaks
    Qt6::Widgets
    mkpsxiso
    dumpsxiso)