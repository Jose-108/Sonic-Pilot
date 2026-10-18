cmake_minimum_required(VERSION 3.22)
project(WingmanStudio VERSION 0.2.0 LANGUAGES CXX)

# JUCE is fetched by the cloud workflow. For local builds, pass -DJUCE_DIR=/path/to/JUCE.
if(DEFINED JUCE_DIR)
  add_subdirectory("${JUCE_DIR}" JUCE)
else()
  include(FetchContent)
  FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE)
  FetchContent_MakeAvailable(JUCE)
endif()

juce_add_plugin(WingmanStudio
    COMPANY_NAME "Wingman Studio"
    BUNDLE_ID "studio.wingman.WingmanStudio"
    PLUGIN_MANUFACTURER_CODE WgSt
    PLUGIN_CODE WgS2
    FORMATS VST3 Standalone
    PRODUCT_NAME "Wingman Studio"
    DESCRIPTION "Musical idea generator and MIDI instrument"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)
juce_generate_juce_header(WingmanStudio)
target_sources(WingmanStudio PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)
target_compile_features(WingmanStudio PRIVATE cxx_std_20)
target_compile_definitions(WingmanStudio PUBLIC
    JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(WingmanStudio PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_dsp
    juce::juce_gui_extra
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
