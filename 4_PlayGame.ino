#define MOVE_COUNT 3
#define SHOW_MOVE_DURATION 1000
#define PAUSE_BETWEEN_SHOWN_MOVES 500

#define ELeaderState byte
#define ETileState byte

enum leaderStates { LEADER_SYNC_STATE, PAUSING_BEFORE_FIRST_SHOW_MOVE, SENDING_SHOW_MOVE, WAITING_FOR_TILE_ACK, WAITING_FOR_TILE_DONE, PAUSING_BEFORE_NEXT_SHOW_MOVE, WAITING_FOR_PLAYER_MOVE };
enum tileStates { WAITING_TO_SHOW_MOVE, SENDING_SHOW_MOVE_ACK, WAITING_FOR_SHOW_MOVE_DONE };

class PlayGame : public GameState {
  private:
    ELeaderState leaderState;
    ETileState tileState;
    byte moves[MOVE_COUNT];
    Timer timer;

    byte moveCount;

    // Showing moves state
    byte currentShownMove;
    byte currentTile;

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
      tileState = WAITING_TO_SHOW_MOVE;
      for (byte i = 0; i < MOVE_COUNT; i++) { moves[MOVE_COUNT] = 0; }
      moveCount = 1;
      currentTile = 0;
      
      if (game->isLeader) {
        setupMoves();
        assignedColor = OFF;
      } else {
        assignedColor = game->color;
      }

      game->color = GREEN;
    }
    
    virtual void broadcastCurrentState() {
      bool leaderBroadcastState = game->isLeader && leaderState == LEADER_SYNC_STATE;
      bool tileBroadcastState = !game->isLeader && tileState == WAITING_TO_SHOW_MOVE;
      
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
//        case WAITING_FOR_TILE_DONE:
//          waitingForTileDone();
//          break;
//        case PAUSING_BEFORE_NEXT_SHOW_MOVE:
//          pauseBeforeShowingNextMove();
//          break;
      }

      // wait for player input
      // track current move
      // wait for signal from tile
      // check face to current move
      // if correct, move to next move
      // if wrong, game over
      // if 10 seconds elapses, game over
    }

    void synchronizeState() {      
      byte neighborsInPlayGameState = 0;
      FOREACH_FACE(face) {        
        if (getNeighborState(face) == PLAY_GAME) {
          Serial.print("found playing neighbor");
          neighborsInPlayGameState += 1;
        }
      }

      Serial.print(neighborsInPlayGameState);
      if (neighborsInPlayGameState == REQUIRED_NEIGHBOR_COUNT) {
        leaderState = PAUSING_BEFORE_FIRST_SHOW_MOVE;
        timer.set(3000);
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
        leaderState = WAITING_FOR_TILE_DONE;
      } else {
        setColorOnFace(RED, currentTile);
      }
    }

    void waitingForTileDone() {
      if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_DONE_FLAG) {
        leaderState = PAUSING_BEFORE_NEXT_SHOW_MOVE;
        timer.set(PAUSE_BETWEEN_SHOWN_MOVES);
      } else {
        setColorOnFace(BLUE, currentTile);
      }
    }

    void pauseBeforeShowingNextMove() {
      if (timer.isExpired()) {
        setColorOnFace(GREEN, currentTile);
        currentShownMove++;
        if (currentShownMove == MOVE_COUNT) {
          leaderState = WAITING_FOR_PLAYER_MOVE;
        } else {
          leaderState = SENDING_SHOW_MOVE;
        }
      } else {
        setColorOnFace(YELLOW, currentTile);
      }
    }
    
    ////////////////
    // GameTile Loop
    ////////////////

    void runTile() {
      switch(tileState) {
        case WAITING_TO_SHOW_MOVE:
          waitToShowMove();
          break;
        case WAITING_FOR_SHOW_MOVE_DONE:
          waitForShowMoveDone();
          break;
      }
    }

    void waitToShowMove() {
      if (getLastValueReceivedOnFace(game->leaderFace) == SHOW_MOVE_FLAG) {
        game->color = assignedColor;
        showColor();
        setValueSentOnFace(SHOW_MOVE_FLAG, game->leaderFace);
        timer.set(SHOW_MOVE_DURATION);
        tileState = WAITING_FOR_SHOW_MOVE_DONE;
      } else {
        setColorOnFace(RED, game->leaderFace);
      }
    }

    void waitForShowMoveDone() {
      if (timer.isExpired()) {
        setValueSentOnFace(SHOW_MOVE_DONE_FLAG, game->leaderFace);
      } else {
        setColorOnFace(BLUE, game->leaderFace);
      }
    }

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
};
