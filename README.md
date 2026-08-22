# KeyRinger

![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino-green)

A Wemos D1 Mini-based melody player that shuffles and plays a collection of classic rock RTTTL ringtones through a buzzer shield. Perfect for adding sound effects, alarms, or simply enjoying nostalgic rock melodies in your projects.

This project transforms your Wemos D1 Mini into a standalone rock melody jukebox that continuously plays a randomized sequence of classic rock songs. The device automatically shuffles the playlist, plays each melody through the Wemos Buzzer Shield, and restarts the cycle with a new random order when all songs have been played.

Designed for makers and hobbyists, this project requires no external dependencies other than the AnyRTTTL library and provides a simple, plug-and-play solution for adding musical capabilities to your ESP8266 projects.

## Features

- **Classic Rock Melodies**: Includes recognizable hits from artists like Deep Purple, Europe, The Beatles, Van Halen, and more
- **Randomized Playback**: Songs are automatically shuffled using the Fisher-Yates algorithm for varied listening experience
- **Continuous Loop**: Automatically reshuffles and restarts when all songs have been played
- **RTTTL Format Support**: Uses the compact Ring Tone Text Transfer Language format for efficient melody storage
- **Minimal Hardware**: Requires only an ESP8266 board and a piezo buzzer or buzzer shield
- **Lightweight**: Optimized for resource-constrained devices with minimal RAM usage

## Hardware

### Required Components

- Wemos D1 Mini (or any ESP8266-based board)
- Wemos Buzzer Shield (or any piezo buzzer connected to GPIO14/D5)
- USB Cable for programming and power
- (Optional) Breadboard and jumper wires if not using the shield

### Supported Boards

The code is tested on the **Wemos D1 Mini** but should work on any ESP8266-based board with:
- GPIO14 (D5) available for PWM output
- At least 1MB of flash memory
- 5V or 3.3V power supply for the buzzer

### Pin Configuration

| Component | Wemos D1 Mini Pin | GPIO | Description |
|-----------|-------------------|------|-------------|
| Buzzer Shield | D5 | GPIO14 | PWM output for tone generation |
| Built-in LED | Built-in | GPIO2 | Optional status indication |

**Note**: If you're not using the Wemos Buzzer Shield, connect a piezo buzzer with one leg to GPIO14 and the other to GND. Some buzzers may require a current-limiting resistor (typically 100-220 ohms) in series.

## Wiring

### Using Wemos Buzzer Shield

1. Plug the Wemos Buzzer Shield directly onto the Wemos D1 Mini
2. Ensure the shield is properly aligned with the pins
3. No additional wiring required

### Using Piezo Buzzer (Without Shield)

| Buzzer Pin | Wemos D1 Mini Pin | Description |
|------------|-------------------|-------------|
| Positive | D5 (GPIO14) | Signal/PWM output |
| Negative | GND | Ground connection |

**Optional**: Add a 100-220 ohm resistor in series with the buzzer to limit current and protect the GPIO pin.

## Flashing

### Method 1: Pre-compiled Binary

1. Download the latest binary from the Releases page

2. Install esptool:

         pipx install esptool

3. Using esptool:

         esptool --port /dev/ttyUSB0 erase-flash
         esptool --port /dev/ttyUSB0 write-flash 0x0 KeyRinger.ino.merged.bin


Replace /dev/ttyUSB0 with your actual serial port.

### Method 2: Using Arduino IDE

1. **Install ESP8266 Core**:
   - Open Arduino IDE
   - Go to File > Preferences
   - Add http://arduino.esp8266.com/stable/package_esp8266com_index.json to Additional Boards Manager URLs
   - Go to Tools > Board > Boards Manager
   - Search for ESP8266 and install esp8266 by ESP8266 Community (v3.1.2 or later)

2. **Install Required Libraries**:
   - Open Sketch > Include Library > Manage Libraries
   - Search for and install:
     - **AnyRTTTL** by Armin Joachimsmeyer (or any compatible RTTTL library)
   - Note: You may need to download the library manually from GitHub if not available in the Library Manager

3. **Configure and Upload**:
   - Open KeyRinger.ino in Arduino IDE
   - Select your board: Tools > Board > ESP8266 Modules > LOLIN(WEMOS) D1 R2 & mini
   - Select the correct port: Tools > Port > [Your USB Port]
   - Select "Erase Flash: Sketch Only"
   - Select "Flash Size: 4MB (FS:2MB OTA:~1019KB)"
   - Click Upload (arrow icon) to compile and flash

4. **Monitor Serial Output**:
   - Open Tools > Serial Monitor
   - Set baud rate to 115200
   - Observe startup messages and current song information

## Startup

After startup, the device will:
1. Initialize the buzzer pin and serial communication
2. Seed the random number generator using the ESP8266 cycle counter
3. Generate a shuffled play order of all 20 melodies
4. Begin playing songs in random order
5. Display "Now Playing: [Song Name]" on the serial monitor
6. Pause for 1 second between songs
7. Reshuffle and restart when all songs have been played

## Configuration

### Customizing Melodies

To add or remove melodies:

1. **Add a new melody**: 
   - Find the RTTTL string for your desired song
   - Add it to the `rockMelodies[]` array in the format: `{ "Song Name - Artist", "rtttl:data" }`

2. **Remove a melody**:
   - Simply delete the line from the `rockMelodies[]` array
   - The `total_melodies` constant will automatically adjust

3. **RTTTL Format Reference**:

    Name:d=note_duration,o=octave,b=bpm:notes

   - d: Default note duration (4=quarter note, 8=eighth note, etc.)
   - o: Octave number (4-6 typically)
   - b: Beats per minute (tempo)
   - Notes: Note names (a-g) with optional modifiers (# for sharp, . for dotted notes)

### Adjusting Playback

| Parameter | Location | Default | Description |
|-----------|----------|---------|-------------|
| Buzzer Pin | `pin_buzzer` | 14 | GPIO pin connected to buzzer |
| Playback Pause | `delay(1000)` | 1000ms | Pause between songs |
| Serial Baud Rate | `Serial.begin()` | 115200 | Serial monitor baud rate |

### Random Seed

The random number generator is seeded using:

      randomSeed(ESP.getCycleCount());

This provides a good source of entropy for true randomness based on the ESP8266's cycle counter.

## Adding New Melodies

### Finding RTTTL Melodies

1. Search online for "RTTTL ringtones" or "RTTTL library"
2. Many websites offer downloadable RTTTL files for popular songs
3. The RTTTL format is standard and widely used in mobile phones and IoT projects

### Converting MIDI to RTTTL

1. Obtain a MIDI file of the song
2. Use a MIDI to RTTTL converter tool (available online)
3. Adjust tempo and octave settings as needed
4. Test the converted RTTTL string in the code

### Testing Melodies

To test a specific melody without shuffling:
1. Comment out the `shufflePlayOrder()` call in `setup()`
2. Set `playOrder[0]` to the index of your melody
3. Remove the loop logic to play only once

## License

This project is released under the GNU General Public License v3.0 (GPL-3.0). See the LICENSE file for details.

## Notes

### Important Considerations

- **Power Requirements**: The Wemos D1 Mini typically requires 5V via USB
- **Buzzer Volume**: The buzzer volume depends on the buzzer type and power supply
- **Blocking Playback**: The device cannot perform other tasks while playing a melody
- **Memory Usage**: All melodies are stored in PROGMEM (flash) to save RAM
- **No User Input**: The device runs autonomously without buttons or controls

### Performance and Limitations

- **Memory Usage**: Each melody string is stored in flash memory (PROGMEM)
- **RAM Usage**: Minimal RAM usage as melodies are read directly from flash
- **CPU Usage**: The CPU is fully occupied during melody playback (blocking)
- **Playback Duration**: Total playlist time varies by melody length (typically 10-60 seconds per song)
- **Shuffle Algorithm**: Fisher-Yates algorithm ensures fair distribution

### Troubleshooting

| Issue | Solution |
|-------|----------|
| No sound from buzzer | Check buzzer connection; verify GPIO14 is correctly used; check if buzzer is properly connected |
| Buzzer sound is too quiet | Add a transistor amplifier; use a louder buzzer; increase voltage (within limits) |
| Buzzer sound is distorted | Add a current-limiting resistor; check power supply stability |
| Serial monitor shows nothing | Check USB connection; verify correct port; ensure baud rate is set to 115200 |
| Device resets unexpectedly | Check power supply; ensure USB cable can provide enough current |
| Melodies sound wrong | Verify RTTTL string format; check tempo and octave settings |
| ESP8266 doesn't boot | Erase flash completely; reflash with correct board settings |
| Compilation errors | Ensure AnyRTTTL library is installed correctly; check library version compatibility |
| Randomization seems predictable | Check random seed; add additional entropy sources |
| Built-in LED stays on | Normal behavior; LED status is not used in this project |

## Disclaimer

The melodies included in this project are for educational and personal use only. Please respect copyright laws and ensure you have the right to use any copyrighted material in your projects.