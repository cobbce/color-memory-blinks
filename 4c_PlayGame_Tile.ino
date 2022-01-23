#define ETileState byte

#define SHOW_MOVE_DURATION 1100

enum tileStates { TILE_SYNC_STATE, WAIT_FOR_SHOW_MOVE, TILE_WAIT_FOR_SHOW_MOVE_DONE, TILE_WAIT_FOR_PLAYER_MOVE, WAIT_FOR_PLAYER_MOVE_ACK };

class PlayGameTile : public GameState {
  public:
    ETileState tileState;
    Color assignedColor;
    bool moveAcked;
    Timer timer;

    PlayGameTile(Game &game) : GameState(game, PLAY_GAME, GAME_OVER) {
      init();
    }

    void init() override {
      tileState = TILE_SYNC_STATE;
      assignedColor = game->assignedTileColor;
      moveAcked = false;
      
      setColor(OFF);
    }
    
    void broadcastCurrentState() override {
      if (tileState == TILE_SYNC_STATE) {
        setValueSentOnAllFaces(state);
      }
    }

    void loopForState() override {
      runTile();
    }

    ////////////////
    // Tile
    ////////////////
    
    void runTile();
    void tileSyncState();

    // move preview
    void waitToShowMove();
    void waitForShowMoveDone();

    // player move
    void waitForPlayerMove();
    void waitForPlayerMoveAck();

    // helpers
    void showAssignedColor();
    void turnOffColor();
};
void PlayGameTile::runTile() {
  switch(tileState) {
    case TILE_SYNC_STATE:
      tileSyncState();
      break;
    case WAIT_FOR_SHOW_MOVE:
      waitToShowMove();
      break;
    case TILE_WAIT_FOR_SHOW_MOVE_DONE:
      waitForShowMoveDone();
      break;
    case TILE_WAIT_FOR_PLAYER_MOVE:
      waitForPlayerMove();
    case WAIT_FOR_PLAYER_MOVE_ACK:
      waitForPlayerMoveAck();
  }
}

void PlayGameTile::tileSyncState() {
  if (getLastValueReceivedOnFace(game->leaderFace) == SYNC_STATE_DONE) {
    tileState = WAIT_FOR_SHOW_MOVE;
  }
}

void PlayGameTile::waitToShowMove() {
  if (getLastValueReceivedOnFace(game->leaderFace) == SHOW_MOVE_FLAG) {
    setValueSentOnFace(SHOW_MOVE_FLAG, game->leaderFace);
    showAssignedColor();
    timer.set(SHOW_MOVE_DURATION);
    tileState = TILE_WAIT_FOR_SHOW_MOVE_DONE;
  } else if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_TURN_STARTED) {
    tileState = TILE_WAIT_FOR_PLAYER_MOVE;
  }
}

void PlayGameTile::waitForShowMoveDone() {
  if (timer.isExpired()) {
    setValueSentOnFace(SHOW_MOVE_DONE_FLAG, game->leaderFace);
    turnOffColor();
    tileState = WAIT_FOR_SHOW_MOVE;
  }
}

void PlayGameTile::waitForPlayerMove() {
  if (buttonSingleClicked()) {
    setValueSentOnFace(PLAYER_MOVED_FLAG, game->leaderFace);
    tileState = WAIT_FOR_PLAYER_MOVE_ACK;
    timer.set(ACK_MOVE_DURATION);
    showAssignedColor();
  } else if (getLastValueReceivedOnFace(game->leaderFace) == GAME_WON) {
    game->isWinner = true;
  }
}

void PlayGameTile::waitForPlayerMoveAck() {
  if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_MOVED_ACK) {
    setValueSentOnFace(PLAY_GAME, game->leaderFace);
    moveAcked = true;
  }
  if (moveAcked && timer.isExpired()) {
    turnOffColor();
    moveAcked = false;
    tileState = TILE_WAIT_FOR_PLAYER_MOVE;
  }
}

void PlayGameTile::showAssignedColor() {
  setColor(assignedColor);
}

void PlayGameTile::turnOffColor() {
  setColor(OFF);
}
