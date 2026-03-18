cmake_minimum_required(VERSION 3.22)
project(EMVerb VERSION 1.0.0)

add_subdirectory(/Users/sanokeigo/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(EMVerb
    PLUGIN_MANUFACTURER_CODE K5sn
    PLUGIN_CODE EmVb
    COMPANY_NAME "K5SANO"
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "EMVerb"
)

juce_generate_juce_header(EMVerb)

target_sources(EMVerb PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PresetManager.cpp
    Source/DSP/DattorroReverb.cpp
)

target_include_directories(EMVerb PRIVATE
    Source
    Source/DSP
)

target_compile_features(EMVerb PRIVATE cxx_std_17)

target_compile_definitions(EMVerb PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

if(MSVC)
    target_compile_definitions(EMVerb PRIVATE _USE_MATH_DEFINES)
endif()

target_link_libraries(EMVerb
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_processors
        juce::juce_audio_plugin_client
        juce::juce_audio_devices
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
)

# --- Tests ---

option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()

    find_package(Catch2 3 QUIET)
    if(Catch2_FOUND)
        add_executable(EMVerbTests
            Tests/ReverbTests.cpp
            Source/DSP/DattorroReverb.cpp
        )
        target_include_directories(EMVerbTests PRIVATE
            Source
            Source/DSP
        )
        target_compile_features(EMVerbTests PRIVATE cxx_std_17)
        target_link_libraries(EMVerbTests PRIVATE
            Catch2::Catch2WithMain
        )
        if(MSVC)
            target_compile_definitions(EMVerbTests PRIVATE _USE_MATH_DEFINES)
        endif()

        include(Catch)
        catch_discover_tests(EMVerbTests)
    endif()
endif()
