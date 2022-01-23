#define GAME_START_DELAY 2500

class SetupGame : public GameState {
  private:
    bool neighborsInSetupState;
    bool setupComplete;
    Timer gameStartTimer;
  
  public:
    SetupGame(Game &game) : GameState(game, SETUP_GAME, PLAY_GAME) {
      init();
    }

    void init() {
      game->color = OFF;
      neighborsInSetupState = false;
      setupComplete = false;
    }
    
    void loopForState();
    void broadcastCurrentState();
    
  private:
    void setupLeader();
    void setupNeighbor();
    bool neighborsAreInSetupState();
    void sendNeighborColors();
};

void SetupGame::loopForState() {
  if (game->isLeader) {
    setupLeader();
  } else {
    setupNeighbor();
  }
}

void SetupGame::broadcastCurrentState() {
  bool leaderWaitingForNeighbors = game->isLeader && !neighborsInSetupState;
  bool neighborWaitingForColorAssignment = !game->isLeader && !setupComplete;
  
  if (leaderWaitingForNeighbors || neighborWaitingForColorAssignment) {
    setValueSentOnAllFaces(state);
  }
}

////////////////
// Leader
////////////////

void SetupGame::setupLeader() {
  if (setupComplete) {
    if (gameStartTimer.isExpired()) {
      changeState(PLAY_GAME);
    }
    return;
  }
    
  if (!neighborsAreInSetupState()) {
    return;
  }
  sendNeighborColors();
}

bool SetupGame::neighborsAreInSetupState() {
  if (neighborsInSetupState) {
    return true;
  }
  
  byte neighborsReadyCount = 0;
  FOREACH_FACE(face) {
    if (EMPTY == game->neighbors[face]) {
      continue;
    }
    
    if (getNeighborState(face) == SETUP_GAME) {
      game->neighbors[face] = IN_SETUP;
      neighborsReadyCount++;
    }
  }
  
  if (neighborsReadyCount == REQUIRED_NEIGHBOR_COUNT) {
    neighborsInSetupState = true;
//    Serial.println("neighborsInSetupState true");
  }
  return neighborsInSetupState;
}

void SetupGame::sendNeighborColors() {
  byte neighborsAcknowledged = 0;
  
  FOREACH_FACE(face) {
    if (EMPTY == game->neighbors[face]) {
      continue;
    }

    // send color assignment to tile
    byte colorIndex = getColorIndex(face);
    setValueSentOnFace(colorIndex, face);

    if (neighborConfirmsColor(face, colorIndex)) {
      game->neighbors[face] = READY_TO_PLAY;
      neighborsAcknowledged++;
    }
  }

  if (neighborsAcknowledged == REQUIRED_NEIGHBOR_COUNT) {
//    Serial.println("leader setup complete");
    setupComplete = true;
    gameStartTimer.set(GAME_START_DELAY);
  }
}

bool neighborConfirmsColor(const byte& face, const byte& colorIndex) {
  return getLastValueReceivedOnFace(face) == colorIndex;
}

////////////////
// Neighbor
////////////////

void SetupGame::setupNeighbor() {
  if (setupComplete) {
    return;
  }
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      byte neighborState = getNeighborState(face);

      // check for assigned color from leader
      if (neighborState >= COLOR_OFFSET && neighborState <= COLOR_OFFSET_END) {
        game->leaderFace = face;
        setupComplete = true;
        game->color = getColorByIndex(neighborState);
        showColor();

        // send color index back to acknowledge
        setValueSentOnFace(neighborState, face);
      }
    }
  }
}
