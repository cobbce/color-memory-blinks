#define MOVE_COUNT 3
#define SHOW_MOVE_DURATION 1100
#define PAUSE_BETWEEN_SHOWN_MOVES 300
#define PLAY_GAME_DELAY 2000

#define ELeaderState byte
#define ETileState byte

enum leaderStates { LEADER_SYNC_STATE, PAUSING_BEFORE_FIRST_SHOW_MOVE, SENDING_SHOW_MOVE, WAITING_FOR_TILE_ACK, WAITING_FOR_TILE_DONE, PAUSING_BEFORE_NEXT_SHOW_MOVE, WAITING_FOR_PLAYER_MOVE, ENDING_GAME, END_GAME };
enum tileStates { TILE_SYNC_STATE, WAITING_TO_SHOW_MOVE, WAITING_FOR_SHOW_MOVE_DONE, TILE_WAITING_FOR_PLAYER_MOVE, WAIT_FOR_PLAYER_MOVE_ACK };

class PlayGame : public GameState {
  private:
    ELeaderState leaderState;
    ETileState tileState;
    byte moves[MOVE_COUNT];
    Timer timer;

    // Showing moves state
    byte currentShownMove;
    byte currentTile;

    // Player action state
    byte expectedPlayerMoveIndex;
    byte lastClickedFace;

    // tile state
    Color assignedColor;
    
  public:
    PlayGame(Game &game) : GameState(game, PLAY_GAME, GAME_OVER) {
      init();
    }

    ////////////////
    // Init
    ////////////////

    void init() {
      leaderState = LEADER_SYNC_STATE;
      tileState = TILE_SYNC_STATE;
      for (byte i = 0; i < MOVE_COUNT; i++) { moves[MOVE_COUNT] = 0; }
      
      // Showing moves state
      currentShownMove = 0;
      currentTile = 0;
      
      // Player action state
      expectedPlayerMoveIndex = 0;
      lastClickedFace = EMPTY;
      
      if (game->isLeader) {
        setupMoves();
        assignedColor = OFF;
      } else {
        assignedColor = game->color;
      }

      game->color = OFF;
    }
    
    virtual void broadcastCurrentState() {
      bool leaderBroadcastState = game->isLeader && leaderState == LEADER_SYNC_STATE;
      bool tileBroadcastState = !game->isLeader && tileState == TILE_SYNC_STATE;
      
      if (leaderBroadcastState || tileBroadcastState) {
        setValueSentOnAllFaces(state);
      }
    }

    void setupMoves() {
      byte gameTile[REQUIRED_NEIGHBOR_COUNT];
      int tileOffset = 0;
      
      FOREACH_FACE(face) {
        if (game->neighbors[face] != EMPTY) {
          gameTile[tileOffset] = face;
          tileOffset++;
        }
      }
      
      for (byte i = 0; i < MOVE_COUNT; i++) {
        byte pickedTile = random(REQUIRED_NEIGHBOR_COUNT - 1);
        moves[i] = gameTile[pickedTile];
      }
    }
    
    ////////////////
    // Loop
    ////////////////

    void loopForState() {
      if (game->isLeader) {
        checkButtonForStateChange(GAME_OVER);
        runGame();
      } else {
        runTile();
      }
    }
    
    ////////////////
    // Leader Loop
    ////////////////

    void runGame() {
      switch(leaderState) {
        case LEADER_SYNC_STATE:
          synchronizeState();
          break;
        case PAUSING_BEFORE_FIRST_SHOW_MOVE:
          pauseBeforeFirstShowMove();
          break;
        case SENDING_SHOW_MOVE:
          sendShowMove();
          break;
        case WAITING_FOR_TILE_ACK:
          waitingForTileAck();
          break;
        case WAITING_FOR_TILE_DONE:
          waitingForTileDone();
          break;
        case PAUSING_BEFORE_NEXT_SHOW_MOVE:
          pauseBeforeShowingNextMove();
          break;
        case WAITING_FOR_PLAYER_MOVE:
          waitForPlayerMove();
          break;
        case ENDING_GAME:
          endingGame();
          break;
        case END_GAME:
          endGame();
          break;
      }
    }

    void synchronizeState() {      
      byte neighborsInPlayGameState = 0;
      FOREACH_FACE(face) {        
        if (getNeighborState(face) == PLAY_GAME) {
          neighborsInPlayGameState += 1;
        }
      }

      if (neighborsInPlayGameState == REQUIRED_NEIGHBOR_COUNT) {
        setValueSentOnAllFaces(SYNC_STATE_DONE);
        leaderState = PAUSING_BEFORE_FIRST_SHOW_MOVE;
        timer.set(PLAY_GAME_DELAY);
      }
    }

    void pauseBeforeFirstShowMove() {
      if (timer.isExpired()) {
        leaderState = SENDING_SHOW_MOVE;
      }
    }

    void sendShowMove() {
      currentTile = moves[currentShownMove];
      setValueSentOnFace(SHOW_MOVE_FLAG, currentTile);
      leaderState = WAITING_FOR_TILE_ACK;
    }

    void waitingForTileAck() {
      if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_FLAG) {
        setValueSentOnFace(PLAY_GAME, currentTile);
        leaderState = WAITING_FOR_TILE_DONE;
      }
    }

    void waitingForTileDone() {
      if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_DONE_FLAG) {
        leaderState = PAUSING_BEFORE_NEXT_SHOW_MOVE;
        timer.set(PAUSE_BETWEEN_SHOWN_MOVES);
      } else {
        debugPrintFace(WHITE, currentTile);
      }
    }

    void sendPlayerTurnStarted() {
      setValueSentOnAllFaces(PLAYER_TURN_STARTED);
    }
    
    void pauseBeforeShowingNextMove() {
      if (timer.isExpired()) {
        currentShownMove++;
        if (currentShownMove == MOVE_COUNT) {
          expectedPlayerMoveIndex = 0;
          leaderState = WAITING_FOR_PLAYER_MOVE;
          sendPlayerTurnStarted();
        } else {
          leaderState = SENDING_SHOW_MOVE;
        }
        setColor(OFF);
      } else {
        debugPrintFace(WHITE, currentTile);
      }
    }

    void updateLastClickedFace(const byte& face) {
      if (lastClickedFace != EMPTY) {
        setValueSentOnFace(PLAY_GAME, lastClickedFace);
      }
      lastClickedFace = face;
    }

    void setClickDelay() {
      timer.set(150);
    }

    void gameOver(bool isWinner) {
      game->isWinner = isWinner;
      leaderState = ENDING_GAME;
      timer.set(50);
    }
      
    void waitForPlayerMove() {
      if (!timer.isExpired()) {
        return;
      }
      FOREACH_FACE(face) {
        if (getLastValueReceivedOnFace(face) == PLAYER_MOVED_FLAG) {
          Serial.print("clicked: "); Serial.println(face);
          setValueSentOnFace(PLAYER_MOVED_ACK, face);
          updateLastClickedFace(face);
          setClickDelay();
          if (face == moves[expectedPlayerMoveIndex]) {
            expectedPlayerMoveIndex++;
            if (expectedPlayerMoveIndex == MOVE_COUNT) {
              gameOver(true);
            }
          } else {
              gameOver(false);
          }
        }
      }
    }

    void endingGame() {
      if(timer.isExpired()) {
        if(game->isWinner) {
          setValueSentOnAllFaces(GAME_WON);
          timer.set(100);
        }
      }
    }

    void endGame() {
      if(timer.isExpired()) {
        changeState(GAME_OVER);
      }
    }
    
    ////////////////
    // GameTile Loop
    ////////////////

    void runTile() {
      switch(tileState) {
        case TILE_SYNC_STATE:
          tileSyncState();
          break;
        case WAITING_TO_SHOW_MOVE:
          waitToShowMove();
          break;
        case WAITING_FOR_SHOW_MOVE_DONE:
          waitForShowMoveDone();
          break;
        case TILE_WAITING_FOR_PLAYER_MOVE:
          tileWaitForPlayerMove();
        case WAIT_FOR_PLAYER_MOVE_ACK:
          waitForPlayerMoveAck();
      }
    }

    void tileSyncState() {
      if (getLastValueReceivedOnFace(game->leaderFace) == SYNC_STATE_DONE) {
        tileState = WAITING_TO_SHOW_MOVE;
      }
    }

    void showAssignedColor() {
      game->color = assignedColor;
      showColor();
    }

    void turnOffColor() {
        game->color = OFF;
        showColor();
    }

    void waitToShowMove() {
      if (getLastValueReceivedOnFace(game->leaderFace) == SHOW_MOVE_FLAG) {
        setValueSentOnFace(SHOW_MOVE_FLAG, game->leaderFace);
        showAssignedColor();
        timer.set(SHOW_MOVE_DURATION);
        tileState = WAITING_FOR_SHOW_MOVE_DONE;
      } else if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_TURN_STARTED) {
        tileState = TILE_WAITING_FOR_PLAYER_MOVE;
      }
    }

    void waitForShowMoveDone() {
      if (timer.isExpired()) {
        setValueSentOnFace(SHOW_MOVE_DONE_FLAG, game->leaderFace);
        turnOffColor();
        tileState = WAITING_TO_SHOW_MOVE;
      }
    }

    void tileWaitForPlayerMove() {
      if (buttonSingleClicked()) {
        setValueSentOnFace(PLAYER_MOVED_FLAG, game->leaderFace);
        tileState = WAIT_FOR_PLAYER_MOVE_ACK;
        timer.set(300);
        showAssignedColor();
      } else if (getLastValueReceivedOnFace(game->leaderFace) == GAME_WON) {
        game->isWinner = true;
      }
    }

    void waitForPlayerMoveAck() {
      if (getLastValueReceivedOnFace(game->leaderFace) == PLAYER_MOVED_ACK) {
        setValueSentOnFace(PLAY_GAME, game->leaderFace);
        if (timer.isExpired()) {
          turnOffColor();
          tileState = TILE_WAITING_FOR_PLAYER_MOVE;
        }
      }
    }

    ////////////////
    // GameTile Loop
    ////////////////

    void printTiles(byte* tiles) {
      char output[10];
      for(byte i = 0; i < REQUIRED_NEIGHBOR_COUNT; i++) {
          sprintf(output, "%d: %d", i, tiles[i]);
          Serial.println(output);
      }
    }

    void printMoves() {
      char output[10];
      for(byte i = 0; i < MOVE_COUNT; i++) {
          sprintf(output, "%d: %d", i, moves[i]);
          Serial.println(output);
      }
    }

    debugPrintFace(const Color& color, const byte& face) {
        setColorOnFace(color, face);
    }
};
