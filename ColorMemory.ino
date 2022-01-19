#include "Serial.h"

ServicePortSerial Serial;

#define EGameState byte
#define REQUIRED_NEIGHBOR_COUNT 2
#define EMPTY 255
#define COLOR_OFFSET 10
byte BLINK_COUNT = REQUIRED_NEIGHBOR_COUNT + 1;

enum gameStates { WAITING_TO_START, SETUP_GAME, PLAY_GAME, GAME_OVER };

// Defined colors: RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE, OFF

const Color colors[] = {
  MAGENTA,
  CYAN,
  ORANGE,
  GREEN,
  YELLOW,
  BLUE,
};


byte getColorIndex(byte i) {
  return COLOR_OFFSET + i;
}

Color getColorByIndex(byte i) {
  return colors[i - COLOR_OFFSET];
}

void debugPrint(const char* message) {
  Serial.println(message);
}
