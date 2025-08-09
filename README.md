# 🥁 BeatCrafter

**Intelligent MIDI Drum Pattern Generator VST3**

![Version](https://img.shields.io/badge/version-0.1.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)
![Platform](https://img.shields.io/badge/platform-Win%20%7C%20Mac%20%7C%20Linux-lightgrey)

## ✨ Features

- 🎛️ **8 Pattern Slots** - Switch patterns live during performance
- 🎚️ **Dynamic Intensity Control** - Morph patterns from simple to complex (0-100%)
- 🎸 **Multiple Music Styles** - Rock, Metal, Jazz, Funk, Electronic, and more
- 🎹 **Visual Pattern Editor** - 16-step grid with velocity control
- 🎯 **Smart Generation** - AI-assisted pattern creation
- 🎮 **MIDI Output** - General MIDI drum mapping
- 🌙 **Modern Dark UI** - Clean, professional interface

## 🚀 Quick Start

### Build from Source

```bash
# Clone the repo
git clone https://github.com/innermost47/beatcrafter.git
cd beatcrafter

mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## 🎮 Usage

1. Load BeatCrafter in your DAW as a MIDI effect
2. Route the MIDI output to your favorite drum VST/sampler
3. Choose a style from the dropdown
4. Click "Generate" or create your own pattern
5. Adjust intensity with the slider for dynamic variations
6. Use the 8 slots to organize different patterns

## 🎛️ Intensity Levels

- **0-25%**: Clean, basic pattern
- **25-50%**: Ghost notes, subtle variations
- **50-75%**: Double kicks, accents, complexity
- **75-100%**: Maximum intensity, fills, blast beats

## 🥁 General MIDI Drum Map

| Instrument | MIDI Note | Note Name |
|------------|-----------|-----------|
| Kick       | 36        | C1        |
| Snare      | 38        | D1        |
| Hi-Hat     | 42        | F#1       |
| Open HH    | 46        | A#1       |
| Crash      | 49        | C#2       |
| Ride       | 51        | D#2       |
| Tom Hi     | 50        | D2        |
| Tom Low    | 45        | A1        |

## 🛠️ Requirements

- CMake 3.22+
- C++17 compiler
- VST3 SDK (downloaded automatically via JUCE)

## 📸 Screenshots

[Coming soon]

## 🤝 Contributing

Pull requests are welcome! For major changes, please open an issue first.

## 📄 License

MIT License - see LICENSE file for details

## 🙏 Credits

Built with [JUCE Framework](https://juce.com/)