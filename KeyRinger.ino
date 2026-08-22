/**
 * @file KeyRinger.ino
 * @brief ESP8266 buzzer player with shuffled rock playlist.
 *
 * This sketch cycles through a list of rock melodies, shuffles their order,
 * and plays them continuously on the Wemos buzzer shield.
 *
 * @author Igor Ferreira
 * @version 1.0
 * @date 2026
 *
 * @note Tested on Wemos D1 Mini with Arduino IDE 2.3.10 and ESP8266 Arduino Core 3.1.2.
 * @note Designed for the Wemos Buzzer Shield connected to GPIO14 (D5).
 */

#include <Arduino.h>

#include <anyrtttl.h>  // https://github.com/end2endzone/AnyRtttl/releases/tag/2.6.0

/**
 * @defgroup PinConfig Pin Configuration
 * @{
 */

/**
 * @brief Buzzer output pin.
 *
 * GPIO14 (D5 on Wemos D1 Mini) is connected to the passive buzzer shield.
 * The pin is configured as an output and used for tone generation.
 */
constexpr uint8_t pin_buzzer = 14;

/** @} */

/**
 * @defgroup MelodyData Melody Playlist Data
 * @{
 */

/**
 * @struct Melody
 * @brief Metadata and RTTTL payload for one melody.
 *
 * RTTTL compactly stores the title, tempo, octave, and note sequence in a
 * single text string.
 */
struct Melody {
  const char* name;   ///< Song title or artist metadata.
  const char* rtttl;  ///< RTTTL-formatted melody string.
};

/**
 * @brief Rock melody library stored in flash memory.
 *
 * The playlist remains in PROGMEM to preserve RAM on the ESP8266. Each entry
 * follows the RTTTL format:
 * "Name:d=note_duration,o=octave,b=bpm:notes"
 *
 * - d: default note duration
 * - o: octave
 * - b: tempo in beats per minute
 * - notes: pitch sequence with optional sharps, dots and duration overrides
 */
const Melody rockMelodies[] PROGMEM = {
  { "Poison - Alice Cooper", "Poison:d=8,o=5,b=112:d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,d,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,f#,g,c6,f#,g,c6,e,d,c,d,a,d,e6,d,d6,d,a,d,e6,d,d6" },
  { "Agadoo - Black Lace", "Agadoo:d=8,o=5,b=125:b,g#,4e,e,e,4e,e,e,e,e,d#,e,4f#,a,f#,4d#,d#,d#,4d#,d#,d#,d#,d#,c#,d#,4e" },
  { "Barbie Girl - Aqua", "Barbie Girl:d=8,o=5,b=125:g#,e,g#,c#6,4a,4p,f#,d#,f#,b,4g#,f#,e,4p,e,c#,4f#,4c#,4p,f#,e,4g#,4f#" },
  { "Lazy - Deep Purple", "Lazy:d=8,o=5,b=160:d.4,f4,16d4,g4,16f4,d.4,f4,16d4,g4,16f4,d4" },
  { "Smoke on the Water - Deep Purple", "Smoke on the Water:d=4,o=4,b=112:c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c,2p,8p,c,d#,f.,c,d#,8f#,f,p,c,d#,f.,d#,c" },
  { "The Final Countdown - Europe", "The Final Countdown:d=16,o=5,b=125:b,a,4b,4e,4p,8p,c6,b,8c6,8b,4a,4p,8p,c6,b,4c6,4e,4p,8p,a,g,8a,8g,8f#,8a,4g.,f#,g,4a.,g,a,8b,8a,8g,8f#,4e,4c6,2b.,b,c6,b,a,1b" },
  { "Funky Town - Lipps Inc.", "Funky Town:d=8,o=4,b=125:c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6,2p,c6,c6,a#5,c6,p,g5,p,g5,c6,f6,e6,c6" },
  { "Kiss From a Rose - Seal", "Kiss From a Rose:d=4,o=5,b=140:8d4,8e4,f.4,8g4,f4,e4,d4,c4,2d4,8d4,8c4,2d4,8d4,8e4,f.4,8g4,f4,e4,c4,e4,2d.4" },
  { "Stairway to Heaven - Led Zeppelin", "Stairway to Heaven:d=8,o=5,b=63:a4,c,e,a,b,e,c,b,c6,e,c,c6,f#,d,a4,f#,e.,16c,a4,4e,c,a4,e,g4,a4,4a4" },
  { "Let It Be - The Beatles", "Let It be:d=8,o=5,b=100:16e6,d6,4c6,16e6,g6,a6,g.6,16g6,g6,e6,16d6,c6,16a,g,4e.6,4p,e6,16e6,f.6,e6,e6,d6,16p,16e6,16d6,d6,2c.6." },
  { "Light My Fire - The Doors", "Light My Fire:d=8,o=5,b=140:b,16g,16a,b,d6,c6,b,a,g,a,16f,16a,c6,f6,16d6,16c6,16a#,16g,g#,g,g#,16g,16a,b,c#6,16b,16a,16g,16f,e,f,4a" },
  { "Macarena - Los del Río", "Macarena:d=8,o=5,b=180:f,f,f,4f,f,f,f,f,f,f,f,a,c,c,4f,f,f,4f,f,f,f,f,f,f,d,c,4p,4f,f,f,4f,f,f,f,f,f,f,f,a,4p,2c.6,4a,c6,a,f,4p,2p" },
  { "Scatman - Scatman John", "Scatman:d=16,o=5,b=200:8b,b,32p,8b,b,32p,8b,2d6,p,c#.6,p.,8d6,p,c#6,8b,p,8f#,2p.,c#6,8p,d.6,p.,c#6,b,8p,8f#,2p,32p,2d6,p,c#6,8p,d.6,p.,c#6,a.,p.,8e,2p.,c#6,8p,d.6,p.,c#6,b,8p,8b,b,32p,8b,b,32p,8b,2d6,p,c#.6,p.,8d6,p,c#6,8b,p,8f#,2p.,c#6,8p,d.6,p.,c#6,b,8p,8f#,2p,32p,2d6,p,c#6,8p,d.6,p.,c#6,a.,p.,8e,2p.,c#6,8p,d.6,p.,c#6,a,8p,8e,2p,32p,f#.6,p.,b.,p." },
  { "Take on Me - a-ha", "Take on Me:d=8,o=5,b=160:f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e,f#,e,f#,f#,f#,d,p,b4,p,e,p,e,p,e,g#,g#,a,b,a,a,a,e,p,d,p,f#,p,f#,p,f#,e,e5" },
  { "Teenage Dirtbag - Wheatus", "Teenage Dirt Bag:d=4,o=5,b=200:d#6,e6,g#6,g#6,2f#6,2e6,2g#6,2d#6,2d#6,2e6,d#6,e6,g#6,g#6,2f#6,2e6,2g#6,2d#6,2d#6,2e6,2e6,g#6,g#6,2f#6,2e6,2g#6,2d#6,2d#6,e6,2d#6,2e6" },
  { "Eruption - Van Halen", "Eruption:d=32,o=5,b=120:c#6,16c#,e,g#,16c#,e,g#,16c#,e,g#,16c#,e,c#6,16c#,e,g#,16c#,e,g#,16c#,e,g#,16c#,e,a,16c#,e,a,16c#,e,a,16c#,e,a,16c#,e,a,16c#,e,a,16c#,e,a,16c#,e,a,16c#,d#,a,16d#,f#,a,16d#,f#,a,16d#,f#,a,16d#,f#,a,16d#,f#,a,16d#,f#,b,16d#,f#,b,16d#,f#,b,16e,g#,b,16e,g#,b,16e,g#,b,16e,g#,b,16e,g#,b,16e,g#,b,16e,g#,b,16e,g#,c6,16e,g,b,16e,g,b,16e,g,b,16e,g,b,16e,g,b,16e,g,d6,16e,g,d6,16e,f#,d6,16f#,a,d6,16f#,a,d6,16f#,a,d6,16f#,a,d6,16f#,a,d6,16f#,a,e6,16f#,a,e6,16f#,a,e6,16b,g#,e6,16b,g#,e6,16b,g#,e6,16b,g#,e6,16b,g#,e6,b,16g#,e6,b,16g#,e6,b,16g#,e6,16g#.,e6,g#,16b,e6,16b.,e6,b,16d6,e6,a#,16c#6,e6,a#,16c#6,e6,a,16c6,e6,a,16c6,e6,g#,16b,e6,g#,b,d6,e6,b,16d6,e6,b,16d6,e6,a#,16c#6,e6,a#,16c#6,e6,a,16c6,e6,a,16c6,e6,g#,16b,e6,g#,16b,d6,a,16c6,d6,a,16c6,d6,g#,16b,d6,g#,16b,d6,g,16a#,d6,g,16a#,d6,f#,a,16d6,f#,a,16c6,g,16a#,c6,g,16a#,c6,f#,16a,c6,f#,16a,c6,f,16g#,c6,f,16g#,c6,e,16g,c6,e,16g,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,e,16g,b,e,16g,b,e,16g,b,e,16g,b,e,16g,b,e,16g,b,e,16g,b,e,16g,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,e,16g,b,d#,16f#,b,4e,4a#4,4e.3" },
  { "Bittersweet Symphony - The Verve", "Bittersweet Symphony:d=8,o=5,b=80:b4,d,b4,c,a4,c,p,f,c,f,p,e,c,e,p,b4,d,b4,c,a4,c,p,f,c,f,p,e,c,e" },
  { "Walk of Life - Dire Straits", "Walk of Life:d=8,o=5,b=160:4g.,32p,4g,4p.,d,e,4g,e,4d,4c.,4c,2p,d,e,4g.,4g,4p.,d,e,4g,e,4d,4c.,4c" },
  { "Wannabe - Spice Girls", "Wannabe:d=8,o=5,b=125:16g,16g,16g,16g,g,a,g,e,p,16c,16d,16c,d,d,c,4e,4p,g,g,g,a,g,e,p,4c6,c6,b,g,a,16b,16a,4g" },
  { "Y.M.C.A. - Village People", "YMCA:d=8,o=5,b=160:c#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,a#,2p,a#,g#,f#,g#,a#,4c#6,a#,4c#6,d#6,b,2p,b,a#,g#,a#,b,4d#6,f#6,4d#6,4f.6,4d#.6,4c#.6,4b.,4a#,4g#" }
};

/** @} */

/**
 * @defgroup GlobalVariables Global State
 * @{
 */

/**
 * @brief Total number of entries in the playlist.
 *
 * Computed at compile time from the array size to avoid hardcoded values.
 */
const uint8_t total_melodies = sizeof(rockMelodies) / sizeof(rockMelodies[0]);

/**
 * @brief Randomized playback sequence.
 *
 * Stores the shuffled indexes used to iterate through the playlist without
 * repeating a melody until all songs have been played once.
 */
uint8_t playOrder[total_melodies];

/**
 * @brief Current index inside the shuffled playback sequence.
 */
uint8_t currentIndex = 0;

/** @} */

/**
 * @defgroup CoreFunctions Core Functions
 * @{
 */

/**
 * @brief Shuffles the playback order with the Fisher-Yates algorithm.
 *
 * Initializes the sequence with ordered indexes and applies a uniform random
 * permutation. The current playback position is reset to the start of the list.
 *
 * @note The random number generator must be seeded before use.
 * @note Time complexity is O(n), where n = total_melodies.
 */
void shufflePlayOrder() {
  for (uint8_t i = 0; i < total_melodies; i++) {
    playOrder[i] = i;
  }

  for (uint8_t i = total_melodies - 1; i > 0; i--) {
    uint8_t j = random(i + 1);
    uint8_t temp = playOrder[i];
    playOrder[i] = playOrder[j];
    playOrder[j] = temp;
  }

  currentIndex = 0;
}

/**
 * @brief Plays one melody from the flash-memory playlist.
 *
 * Reads the melody metadata from PROGMEM, prints the title to the serial monitor,
 * and reproduces the RTTTL sequence through the buzzer using the blocking API.
 *
 * @param index Playlist index of the melody to play, in the range [0, total_melodies).
 *
 * @note This function blocks until the melody finishes playing.
 * @see anyrtttl::blocking::playProgMem()
 */
void playSong(uint8_t index) {
  Serial.print("Now Playing: ");
  Serial.println((char*)pgm_read_ptr(&rockMelodies[index].name));

  anyrtttl::blocking::playProgMem(pin_buzzer, rockMelodies[index].rtttl);
}

/**
 * @brief Initializes the buzzer hardware and playback state.
 *
 * Configures the output pin, starts serial communication, seeds the random
 * generator, and creates the initial shuffled playlist order.
 */
void setup() {
  pinMode(pin_buzzer, OUTPUT);
  digitalWrite(pin_buzzer, LOW);

  Serial.begin(115200);
  randomSeed(ESP.getCycleCount());

  shufflePlayOrder();
  delay(1000);
}

/**
 * @brief Main playback loop.
 *
 * Plays the next melody from the shuffled sequence and reshuffles the list when
 * it reaches the end. A brief delay separates songs, while yield() keeps the
 * ESP8266 background tasks responsive.
 */
void loop() {
  playSong(playOrder[currentIndex]);

  currentIndex++;

  if (currentIndex >= total_melodies) {
    shufflePlayOrder();
  }

  delay(1000);
  yield();
}

/** @} */