#include "Serial.h"

ServicePortSerial Serial;

#define GameState byte

////////////
// Constants
////////////

// Defined colors: RED, ORANGE, YELLOW, GREEN, CYAN, BLUE, MAGENTA, WHITE, OFF
Color colors[] = {
  MAGENTA,
  CYAN,
  ORANGE,
  GREEN,
  YELLOW,
  BLUE,
};

byte REQUIRED_NEIGHBOR_COUNT = 2;
byte BLINK_COUNT = REQUIRED_NEIGHBOR_COUNT + 1;

enum gameStates { WAITING_TO_START, SETUP_GAME, PLAY_GAME, GAME_OVER };

#define EMPTY 255

class Game {
  public:
    byte isLeader = false;
    byte neighbors[FACE_COUNT];
};

class IGameState {
  public: 
    IGameState(GameState s, Color c, GameState n) {
      state = s;
      color = c;
      nextState = n;
    }
  
  protected: 
    GameState state;
    Color color;
    GameState nextState;
    
  public:
    virtual void loop() = 0;
    Color getColor();
    GameState getGameState();
    void checkNeighborsForStateChange();

  protected:
    bool hasNeighbor(byte face);
    bool stateChangeDetected(GameState neighborState);
    byte getNeighborState(byte face);
    void checkButtonForStateChange(GameState nextState);
};

class WaitingToStart : public IGameState {
  public:
    WaitingToStart() : IGameState(WAITING_TO_START, YELLOW, SETUP_GAME) {}
    
    void loop();
  private:
    bool requiredNeighborCount();
    bool initializeLeader();
};

class SetupGame : public IGameState {
  public:
    SetupGame() : IGameState(SETUP_GAME, MAGENTA, PLAY_GAME) {}

    void loop();

  private:
    void setupLeader();
    void setupNeighbor();
};

class PlayGame : public IGameState {
  public:
    PlayGame() : IGameState(PLAY_GAME, GREEN, GAME_OVER) {}

    void loop();
};

class GameOver : public IGameState {
  public:
    GameOver() : IGameState(GAME_OVER, RED, WAITING_TO_START) {}

    void loop();
};

////////////////
// State
////////////////

Game game = Game();
IGameState* currentState;

WaitingToStart w = WaitingToStart();
SetupGame s = SetupGame();
PlayGame p = PlayGame();
GameOver g = GameOver();

IGameState* waitingToStart = &w;
IGameState* setupGame = &s;
IGameState* playGame = &p;
IGameState* gameOver = &g;

void broadcastCurrentState() {
  setValueSentOnAllFaces(currentState->getGameState());
}

void changeState(GameState nextState) {
  switch(nextState) {
    case WAITING_TO_START:
      currentState = waitingToStart;
      break;
    case SETUP_GAME:
      currentState = setupGame;
      break;
    case PLAY_GAME:
      currentState = playGame;
      break;
    case GAME_OVER:
      currentState = gameOver;
      break;
    default:
      debugPrint("changeState: Unexpected state encountered");
  }
  setColor(currentState->getColor());
}

void debugPrint(char* message) {
  Serial.println(message);
}

////////////////
// IGameState
////////////////
    
Color IGameState::getColor() {
  return color;
}

GameState IGameState::getGameState() {
  return state;
}
    
void IGameState::checkNeighborsForStateChange() {
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      byte neighborState = getNeighborState(face);
      if (stateChangeDetected(neighborState)) {
        changeState(neighborState);
      }
    }
  }
}

bool IGameState::hasNeighbor(byte face) {
  return !isValueReceivedOnFaceExpired(face);
}

bool IGameState::stateChangeDetected(GameState neighborState) {
  if (neighborState == nextState) {
    return true;
  }
  return false;
}

byte IGameState::getNeighborState(byte face) {
  return getLastValueReceivedOnFace(face);
}

void IGameState::checkButtonForStateChange(GameState nextState) {
  if (buttonSingleClicked()) {
    debugPrint("button pressed");
    changeState(nextState);
  }
}

////////////////
// WaitingToStart
////////////////

void WaitingToStart::loop() {
  if (buttonSingleClicked()) {
      debugPrint("Button clicked in: WaitingToStart");
    if (requiredNeighborCount()) {
      debugPrint("Starting game");
      initializeLeader();
      changeState(SETUP_GAME);
    }
  }
}

bool WaitingToStart::requiredNeighborCount() {
  byte neighborCount = 0;
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      neighborCount++;
    }
  }
  return neighborCount == REQUIRED_NEIGHBOR_COUNT;
}
    
bool WaitingToStart::initializeLeader() {
  game.isLeader = true;
  byte blinkIndex = 0;
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      game.neighbors[blinkIndex] = face;
      blinkIndex++;
    }
  }
}

////////////////
// SetupGame
////////////////

void SetupGame::loop() {
  checkButtonForStateChange(PLAY_GAME);
//  if(game.isLeader) {
//    setupLeader();
//  } else {
//    setupNeighbor();
//  }
}

void SetupGame::setupLeader() {
  
}

void SetupGame::setupNeighbor() {
  
}

////////////////
// PlayGame
////////////////

void PlayGame::loop() {
  checkButtonForStateChange(GAME_OVER);
}

////////////////
// GameOver
////////////////

void GameOver::loop() {
  checkButtonForStateChange(WAITING_TO_START);
}

////////////////
// Main
////////////////

void setup() {
  Serial.begin();
  debugPrint("start: change state to WAITING_TO_START");
  changeState(WAITING_TO_START);
  debugPrint("complete: change state to WAITING_TO_START");
}

void loop() {
  currentState->loop();
  broadcastCurrentState();
  currentState->checkNeighborsForStateChange();
}
