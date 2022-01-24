class Game {
  public:
    //universal state
    bool isWinner;
    
    //leader state
    byte neighbors[FACE_COUNT];
    byte isLeader;
    byte tileCount;

    //tile state
    byte leaderFace;
    Color assignedTileColor;
    
    Game() { init(); }

    void init() {
      isWinner = false;
      FOREACH_FACE(f) { neighbors[f] = EMPTY; }
      isLeader = false;
      tileCount = 0;
      leaderFace = 0;
      assignedTileColor = OFF;
    }
};

class GameState {
  protected: 
    Game* game;
    EGameState state;
    EGameState nextState;
    
  public:

    GameState(Game &g, EGameState s, EGameState n) {
      game = &g;
      state = s;
      nextState = n;
    }

  public:
    virtual void init() = 0;
  protected:
  
    virtual void loopForState() = 0;
    virtual void broadcastCurrentState() = 0;

  public:
    
    void loop() {
      broadcastCurrentState();
      loopForState();
      checkNeighborsForStateChange();
      checkAlone();
    }
    
//    void showColor() {
//      setColor(game->color);
//    }
    
    EGameState getGameState();
    
    void checkButtonForStateChange(EGameState nextState) {
      if (buttonSingleClicked()) {
        changeState(nextState);
      }
    }

  protected:
    
    bool stateChangeDetected(EGameState neighborState) {
      if (neighborState == nextState) {
        return true;
      }
      return false;
    }
    
    void checkNeighborsForStateChange() {
      FOREACH_FACE(face) {
        if (hasNeighbor(face)) {
          byte neighborState = getNeighborState(face);
          if (stateChangeDetected(neighborState)) {
            changeState(neighborState);
          }
        }
      }
    }

    void iterateNeighbors(void (*functor)(Game*, byte)) {
      FOREACH_FACE(face) {
        if (hasNeighbor(face)) {
          functor(game, face);
        }
      }
    }

    void checkAlone() {
      if (isAlone()) {
        changeState(WAITING_TO_START);
      }
    }

//    void print neighborState() {
//      FOREACH_FACE(face) {
//          char neighborState[10];
//          sprintf(neighborState, "%d: %d", face, game->neighbors[face]);
//          Serial.println(neighborState);
//      }
//    }
};
