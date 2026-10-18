# Wingman Studio — Cloud Build Starter

This is an early source project for a JUCE VST3/Standalone plugin for Windows 11 and Waveform 13 Pro.

## Features in this starter
- JUCE + CMake project; VST3 and Standalone targets.
- Dark teal UI, key/scale and generator controls, gain, tempo, built-in simple sine tone.
- Basic host-transport-driven MIDI note generation.
- Plugin parameter state persistence.

## Not yet implemented
Audio-to-MIDI analysis, proper chord/bass/melody/drum phrase generation, editable piano roll, file import, WAV rendering, aligned stem export, and robust DAW-specific MIDI routing are not complete. This is a starting build, not the finished requested product. The generated MIDI logic is intentionally basic and must be validated and improved before production use.

## Cloud build
The GitHub Actions workflow in `.github/workflows/build.yml` builds the VST3 on a Windows runner and uploads the artifact. You must place this folder in a GitHub repository and enable Actions. The workflow downloads JUCE from its official repository.
