cmake_minimum_required(VERSION 3.22)
project(CloudsReverb VERSION 1.0.0)

add_subdirectory(/Users/sanokeigo/JUCE ${CMAKE_BINARY_DIR}/JUCE)

juce_add_plugin(CloudsReverb
    PLUGIN_MANUFACTURER_CODE K5sn
    PLUGIN_CODE ClRv
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "CloudsReverb"
)

juce_generate_juce_header(CloudsReverb)

target_sources(CloudsReverb PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/PresetManager.cpp
    Source/DSP/DattorroReverb.cpp
)

target_include_directories(CloudsReverb PRIVATE
    Source
    Source/DSP
)

target_compile_features(CloudsReverb PRIVATE cxx_std_17)

target_compile_definitions(CloudsReverb PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

if(MSVC)
    target_compile_definitions(CloudsReverb PRIVATE _USE_MATH_DEFINES)
endif()

target_link_libraries(CloudsReverb
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
        add_executable(CloudsReverbTests
            Tests/ReverbTests.cpp
            Source/DSP/DattorroReverb.cpp
        )
        target_include_directories(CloudsReverbTests PRIVATE
            Source
            Source/DSP
        )
        target_compile_features(CloudsReverbTests PRIVATE cxx_std_17)
        target_link_libraries(CloudsReverbTests PRIVATE
            Catch2::Catch2WithMain
        )
        if(MSVC)
            target_compile_definitions(CloudsReverbTests PRIVATE _USE_MATH_DEFINES)
        endif()

        include(Catch2::Catch)
        catch_discover_tests(CloudsReverbTests)
    endif()
endif()
