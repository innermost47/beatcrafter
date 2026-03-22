cmake_minimum_required(VERSION 3.22)

project(BeatCrafter VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_STANDALONE "Build standalone application" ON)
option(BUILD_VST3 "Build VST3 plugin" ON)
option(BUILD_AU "Build AU plugin" OFF)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.9
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(BeatCrafter
    COMPANY_NAME "Innermost47"
    PLUGIN_MANUFACTURER_CODE BCrf
    PLUGIN_CODE BCr1
    FORMATS VST3 Standalone
    IS_SYNTH FALSE
    IS_MIDI_EFFECT TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    PRODUCES_MIDI_OUTPUT TRUE
    PRODUCT_NAME "BeatCrafter"
    BUNDLE_ID "com.beatcrafter.beatcrafter"
)

target_sources(BeatCrafter
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/Core/Pattern.cpp
        src/Core/PatternEngine.cpp
        src/Core/StyleManager.cpp
        src/GUI/PatternGrid.cpp
        src/GUI/LookAndFeel.cpp
        src/GUI/SlotManager.cpp
        src/Core/Styles/RockStyle.cpp
        src/Core/Styles/MetalStyle.cpp
        src/Core/Styles/JazzStyle.cpp
        src/Core/Styles/FunkStyle.cpp
        src/Core/Styles/ElectronicStyle.cpp
        src/Core/Styles/HipHopStyle.cpp
        src/Core/Styles/LatinStyle.cpp
        src/Core/Styles/PunkStyle.cpp
)

juce_add_binary_data(BeatCrafterData
    SOURCES
        resources/fonts/Audiowide-Regular.ttf 
        resources/fonts/Exo2-Regular.ttf 
        resources/phosphor/shuffle.svg
        resources/phosphor/trash.svg
        resources/phosphor/arrow-counter-clockwise.svg
        resources/phosphor/broadcast.svg
        resources/phosphor/link-simple.svg
        resources/phosphor/ear.svg
        resources/phosphor/link-simple-break.svg
        resources/phosphor/wave-sine.svg
)

set_target_properties(BeatCrafterData PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

target_link_libraries(BeatCrafterData PRIVATE juce::juce_core)

target_compile_definitions(BeatCrafter
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
        JUCE_REPORT_APP_USAGE=0
        JUCE_WIN_PER_MONITOR_DPI_AWARE=1 
)

target_link_libraries(BeatCrafter
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        BeatCrafterData
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

if(MSVC)
    target_compile_options(BeatCrafter PRIVATE /W4)
else()
    target_compile_options(BeatCrafter PRIVATE -Wall -Wextra -Wpedantic)
endif()