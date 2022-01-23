class WaitingToStart : public GameState {
  private:
    
  public:
    WaitingToStart(Game &game) : GameState(game, WAITING_TO_START, SETUP_GAME) {
      init();
    }

    virtual void init() {
      setColor(YELLOW);
      game->init();
    }
    
    virtual void broadcastCurrentState() {
      setValueSentOnAllFaces(state);
    }
    
    virtual void loopForState();
  private:
    bool requiredNeighborCount();
    bool initializeLeader();
    static void countNeighbors(Game* game, byte face);
};

void WaitingToStart::loopForState() {
  if (buttonSingleClicked()) {
    if (requiredNeighborCount()) {
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

static void WaitingToStart::countNeighbors(Game* game, byte face) {
  game->neighbors[face] = EXISTS;
}
    
bool WaitingToStart::initializeLeader() {
  game->isLeader = true;

  iterateNeighbors(WaitingToStart::countNeighbors);
}
