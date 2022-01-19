class Game {
  public:
    byte isLeader = false;
    byte neighbors[FACE_COUNT];
    Game() {
      FOREACH_FACE(f) {
        neighbors[f] = EMPTY;
      }
    }
};

class GameState {
  public: 
    GameState(Game &g, EGameState s, Color c, EGameState n) {
      game = &g;
      state = s;
      color = c;
      nextState = n;
    }
  
  protected: 
    Game* game;
    EGameState state;
    Color color;
    EGameState nextState;
    
  public:
    virtual void initState() {}
    void loop();
    Color getColor();
    void showColor();
    EGameState getGameState();
    void checkNeighborsForStateChange();

  protected:
    virtual void loopForState() = 0;
    virtual void broadcastCurrentState();
    bool hasNeighbor(byte face);
    bool stateChangeDetected(EGameState neighborState);
    byte getNeighborState(byte face);
    void checkButtonForStateChange(EGameState nextState);
};
