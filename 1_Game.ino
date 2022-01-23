class Game {
  public:
    byte isLeader;
    byte leaderFace;
    Color color;
    byte neighbors[FACE_COUNT];
    Game() { init(); }

    void init() {
      isLeader = false;
      leaderFace = 0;
      color = YELLOW;
      FOREACH_FACE(f) { neighbors[f] = EMPTY; }
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
      loopForState();
      broadcastCurrentState();
      checkNeighborsForStateChange();
      checkAlone();
    }
    
    void showColor() {
      setColor(game->color);
    }
    
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
