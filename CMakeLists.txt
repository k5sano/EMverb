cmake\_minimum\_required(VERSION 3.22) project(CloudsReverb VERSION 1.0.0)

find\_package(JUCE CONFIG REQUIRED)

juce\_add\_plugin(CloudsReverb PLUGIN\_MANUFACTURER\_CODE K5sn PLUGIN\_CODE ClRv FORMATS VST3 Standalone PRODUCT\_NAME "CloudsReverb" )

juce\_generate\_juce\_header(CloudsReverb)

target\_sources(CloudsReverb PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp Source/DSP/DattorroReverb.cpp Source/SampleRateAdapter.cpp )

target\_include\_directories(CloudsReverb PRIVATE Source Source/DSP )

target\_compile\_features(CloudsReverb PRIVATE cxx\_std\_17)

if(MSVC) target\_compile\_definitions(CloudsReverb PRIVATE \_USE\_MATH\_DEFINES) endif()

target\_link\_libraries(CloudsReverb PRIVATE juce::juce\_audio\_basics juce::juce\_audio\_processors juce::juce\_audio\_plugin\_client juce::juce\_dsp PUBLIC juce::juce\_recommended\_config\_flags )

# \--- Tests ---

option(BUILD\_TESTS "Build unit tests" ON) if(BUILD\_TESTS) enable\_testing()

```
find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
    add_executable(CloudsReverbTests
        Tests/ReverbTests.cpp
        Source/DSP/DattorroReverb.cpp
        Source/SampleRateAdapter.cpp
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
```

endif()