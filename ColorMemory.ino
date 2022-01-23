#include "Serial.h"

ServicePortSerial Serial;

#define EGameState byte
#define REQUIRED_NEIGHBOR_COUNT 3
#define EMPTY 255
#define EXISTS 1
#define IN_SETUP 2
#define READY_TO_PLAY 3
#define IN_PLAY_GAME 4

#define SYNC_STATE_DONE 9
#define COLOR_OFFSET 10
#define COLOR_OFFSET_END 19
#define SHOW_MOVE_FLAG 20
#define SHOW_MOVE_DONE_FLAG 21
#define PLAYER_TURN_STARTED 22
#define PLAYER_MOVED_FLAG 23
#define PLAYER_MOVED_ACK 24
#define GAME_WON 25


enum gameStates { WAITING_TO_START, SETUP_GAME, PLAY_GAME, GAME_OVER };

// Defined colors: RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE, OFF

byte getColorIndex(byte i) {
  return COLOR_OFFSET + i;
}

Color getColorByIndex(byte i) {
  switch(i) {
    case COLOR_OFFSET:
      return MAGENTA;
    case COLOR_OFFSET + 1:
      return CYAN;
    case COLOR_OFFSET + 2:
      return ORANGE;
    case COLOR_OFFSET + 3:
      return GREEN;
    case COLOR_OFFSET + 4:
      return YELLOW;
    case COLOR_OFFSET + 5:
      return BLUE;
  }
}
    
bool hasNeighbor(byte face) {
  return !isValueReceivedOnFaceExpired(face);
}

byte getNeighborState(byte face) {
  return getLastValueReceivedOnFace(face);
}
