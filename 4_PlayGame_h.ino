#define MOVE_COUNT 4
#define SHOW_MOVE_DURATION 1100
#define PAUSE_BETWEEN_SHOWN_MOVES 300
#define PLAY_GAME_DELAY 2000
#define ACK_MOVE_DURATION 100

#define ELeaderState byte

enum leaderStates { 
    LEADER_SYNC_STATE, 
    PAUSE_BEFORE_FIRST_SHOW_MOVE, SEND_SHOW_MOVE, WAIT_FOR_SHOW_MOVE_ACK, WAIT_FOR_SHOW_MOVE_DONE, PAUSING_BEFORE_NEXT_SHOW_MOVE, 
    WAITING_FOR_PLAYER_MOVE, ACK_PLAYER_MOVE,
    ENDING_GAME, END_GAME
};
enum tileStates { TILE_SYNC_STATE, WAITING_TO_SHOW_MOVE, WAITING_FOR_SHOW_MOVE_DONE, TILE_WAITING_FOR_PLAYER_MOVE, WAIT_FOR_PLAYER_MOVE_ACK };

class PlayGame : public GameState {
  private:
    ELeaderState leaderState;
    byte moves[MOVE_COUNT];
    Timer timer;

    byte currentShownMove;
    byte currentTile;

    byte expectedPlayerMoveIndex;
    byte lastClickedFace;
    
  public:
    PlayGame(Game &game) : GameState(game, PLAY_GAME, GAME_OVER) {
      init();
    }

    void init() {
      leaderState = LEADER_SYNC_STATE;
      for (byte i = 0; i < MOVE_COUNT; i++) { moves[MOVE_COUNT] = 0; }
      
      currentShownMove = 0;
      currentTile = 0;
      
      expectedPlayerMoveIndex = 0;
      lastClickedFace = EMPTY;
      
      setupMoves();
      printArray(moves, MOVE_COUNT);

      game->color = OFF;
    }
    
    virtual void broadcastCurrentState() {
      if (leaderState == LEADER_SYNC_STATE) {
        setValueSentOnAllFaces(state);
      }
    }
    
    virtual void loopForState() {
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

    void setupMoves() {
      byte gameTile[REQUIRED_NEIGHBOR_COUNT];
      int tileOffset = 0;
      
      FOREACH_FACE(face) {
        if (game->neighbors[face] != EMPTY) {
          gameTile[tileOffset++] = face;
        }
      }
      
      for (byte i = 0; i < MOVE_COUNT; i++) {
        byte pickedTile = random(REQUIRED_NEIGHBOR_COUNT - 1);
        moves[i] = gameTile[pickedTile];
      }
    }
};
