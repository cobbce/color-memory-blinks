#define MOVE_COUNT 30
#define PAUSE_BETWEEN_SHOWN_MOVES 100
#define NEW_TURN_DELAY 900
#define ACK_MOVE_DURATION 100

#define ELeaderState byte

enum leaderStates { 
    LEADER_SYNC_STATE, 
    PAUSE_BEFORE_FIRST_SHOW_MOVE, SEND_SHOW_MOVE, WAIT_FOR_SHOW_MOVE_ACK, WAIT_FOR_SHOW_MOVE_DONE, PAUSING_BEFORE_NEXT_SHOW_MOVE, 
    WAITING_FOR_PLAYER_MOVE, ACK_PLAYER_MOVE,
    ENDING_GAME, END_GAME
};

class PlayGame : public GameState {
  private:
    ELeaderState leaderState;
    byte moves[MOVE_COUNT];

    byte currentShownMove;
    byte currentTile;

    byte expectedPlayerMoveIndex;
    byte lastClickedFace;
    
  public:
    PlayGame(Game &game) : GameState(game, PLAY_GAME, GAME_OVER) {
      init();
    }

    void init() override {
      leaderState = LEADER_SYNC_STATE;
      for (byte i = 0; i < MOVE_COUNT; i++) { moves[MOVE_COUNT] = 0; }
      
      game->turnCount = 1;
      currentShownMove = 0;
      currentTile = 0;
      
      expectedPlayerMoveIndex = 0;
      lastClickedFace = EMPTY;
      
      setupMoves();
      printArray(moves, MOVE_COUNT);

      setColor(OFF);
    }
    
    void broadcastCurrentState() override {
      if (leaderState == LEADER_SYNC_STATE) {
        setValueSentOnAllFaces(state);
      }
    }
    
    void loopForState() override {
      checkButtonForStateChange(GAME_OVER);
      runGame();
    }
    
    void runGame();
    
    void synchronizeState();

    // move preview
    void pauseBeforeFirstShowMove();
    void sendShowMove();
    void waitForShowMoveAck();
    void waitForShowMoveDone();
    void pauseBeforeShowingNextMove();

    // player move
    void waitForPlayerMove();
    void ackPlayerMove();
    void endingGame();
    void endGame();

    // helpers
    void sendPlayerTurnStarted();
    void gameOver(bool isWinner);
    void startAckMove(byte face);
    void printArray(byte* byteArray, byte arrayLength);
    void startWaitingForPlayerMove();
    bool isGameOver();
    bool isCorrectMove(byte face);
    void startNewTurn();
    void checkTurnOver();

    void setupMoves() {
      byte gameTile[game->tileCount];
      int tileOffset = 0;
      
      FOREACH_FACE(face) {
        if (game->neighbors[face] != EMPTY) {
          gameTile[tileOffset++] = face;
        }
      }
      
      for (byte i = 0; i < MOVE_COUNT; i++) {
        byte pickedTile = random(tileOffset - 1);
        moves[i] = gameTile[pickedTile];
      }
    }
};

void PlayGame::runGame() {
  switch(leaderState) {
    case LEADER_SYNC_STATE:
      synchronizeState();
      break;
    case PAUSE_BEFORE_FIRST_SHOW_MOVE:
      pauseBeforeFirstShowMove();
      break;
    case SEND_SHOW_MOVE:
      sendShowMove();
      break;
    case WAIT_FOR_SHOW_MOVE_ACK:
      waitForShowMoveAck();
      break;
    case WAIT_FOR_SHOW_MOVE_DONE:
      waitForShowMoveDone();
      break;
    case PAUSING_BEFORE_NEXT_SHOW_MOVE:
      pauseBeforeShowingNextMove();
      break;
    case WAITING_FOR_PLAYER_MOVE:
      waitForPlayerMove();
      break;
    case ACK_PLAYER_MOVE:
      ackPlayerMove();
      break;
    case ENDING_GAME:
      endingGame();
      break;
    case END_GAME:
      endGame();
      break;
  }
}

void PlayGame::synchronizeState() {      
  byte neighborsInPlayGameState = 0;
  FOREACH_FACE(face) {        
    if (getNeighborState(face) == PLAY_GAME) {
      neighborsInPlayGameState += 1;
    }
  }

  if (neighborsInPlayGameState == game->tileCount) {
    setValueSentOnAllFaces(SYNC_STATE_DONE);
    startNewTurn();
  }
}

void PlayGame::startNewTurn() {
    leaderState = PAUSE_BEFORE_FIRST_SHOW_MOVE;
    game->timer.set(NEW_TURN_DELAY);
}

void PlayGame::pauseBeforeFirstShowMove() {
  if (game->timer.isExpired()) {
    leaderState = SEND_SHOW_MOVE;
  }
}

void PlayGame::sendShowMove() {
  currentTile = moves[currentShownMove];
  setValueSentOnFace(SHOW_MOVE_FLAG, currentTile);
  leaderState = WAIT_FOR_SHOW_MOVE_ACK;
}

void PlayGame::waitForShowMoveAck() {
  if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_FLAG) {
    setValueSentOnFace(PLAY_GAME, currentTile);
    leaderState = WAIT_FOR_SHOW_MOVE_DONE;
  }
}

void PlayGame::waitForShowMoveDone() {
  if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_DONE_FLAG) {
    leaderState = PAUSING_BEFORE_NEXT_SHOW_MOVE;
    game->timer.set(PAUSE_BETWEEN_SHOWN_MOVES);
  }
}

void PlayGame::pauseBeforeShowingNextMove() {
  if (game->timer.isExpired()) {
    currentShownMove++;
    if (currentShownMove == game->turnCount) {
      startWaitingForPlayerMove();
    } else {
      leaderState = SEND_SHOW_MOVE;
    }
    setColor(OFF);
  }
}
  
void PlayGame::waitForPlayerMove() {
  FOREACH_FACE(face) {
    if (getLastValueReceivedOnFace(face) == PLAYER_MOVED_FLAG) {
//      Serial.print("clicked: "); Serial.println(face);
      startAckMove(face);
    }
  }
}

void PlayGame::startAckMove(byte face) {
  setValueSentOnFace(PLAYER_MOVED_ACK, face);
  game->timer.set(ACK_MOVE_DURATION);
  lastClickedFace = face;
  leaderState = ACK_PLAYER_MOVE;
}


void PlayGame::ackPlayerMove() {
  if (!game->timer.isExpired()) {
    return;
  }

  setValueSentOnFace(PLAY_GAME, lastClickedFace);

  if (isCorrectMove(lastClickedFace)) {
    if (!isGameOver()) {
      checkTurnOver();
    }
  } else {
    gameOver(false);
  }
  
  lastClickedFace = EMPTY;
}

bool PlayGame::isCorrectMove(byte face) {
  return face == moves[expectedPlayerMoveIndex++];
}

bool PlayGame::isGameOver() {
  if (expectedPlayerMoveIndex == MOVE_COUNT) {
    gameOver(true);
    return true;
  }
  return false;
}

void PlayGame::checkTurnOver() {
  if (expectedPlayerMoveIndex == game->turnCount) {
    game->turnCount++;
    currentShownMove = 0;
    currentTile = 0;
    expectedPlayerMoveIndex = 0;
    setValueSentOnAllFaces(NEW_TURN_STARTED);
    startNewTurn();
  } else {
    leaderState = WAITING_FOR_PLAYER_MOVE;
  }
}

void PlayGame::endingGame() {
  if(game->timer.isExpired()) {
    if(game->isWinner) {
      setValueSentOnAllFaces(GAME_WON);
    }
    game->timer.set(200);
    leaderState = END_GAME;
  }
}

void PlayGame::endGame() {
  if(game->timer.isExpired()) {
    changeState(GAME_OVER);
  }
}

void PlayGame::gameOver(bool isWinner) {
  game->isWinner = isWinner;
  leaderState = ENDING_GAME;
  game->timer.set(100);
}

void PlayGame::startWaitingForPlayerMove() {
    expectedPlayerMoveIndex = 0;
    leaderState = WAITING_FOR_PLAYER_MOVE;
    setValueSentOnAllFaces(PLAYER_TURN_STARTED);
}

void PlayGame::printArray(byte* byteArray, byte arrayLength) {
  for(byte i = 0; i < arrayLength; i++) {
      Serial.print(i); Serial.print(": "); Serial.println(byteArray[i]);
  }
}
