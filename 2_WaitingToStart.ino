#define PULSE_LENGTH 2000

class WaitingToStart : public GameState {
  private:
    Color idleColor;
    
  public:
    WaitingToStart(Game &game) : GameState(game, WAITING_TO_START, SETUP_GAME) {
    }

    virtual void init() {
      idleColor = getColorByIndex(getColorIndex(random(FACE_COUNT - 1)));
      game->init();
    }
    
    virtual void broadcastCurrentState() {
      setValueSentOnAllFaces(state);
    }
    
    virtual void loopForState();
  private:
    bool requiredNeighborCount();
    bool initializeLeader();
    void idlePulse();
    static void countNeighbors(Game* game, byte face);
};

void WaitingToStart::loopForState() {
  idlePulse();
  
  if (buttonSingleClicked()) {
    if (requiredNeighborCount()) {
      initializeLeader();
      changeState(SETUP_GAME);
    }
  }
}

void WaitingToStart::idlePulse() {
  setColor(dim(idleColor, (sin8_C(map(millis() % PULSE_LENGTH, 0, PULSE_LENGTH, 0, 255)) * (int)155 / (int)255) + 100));
}

bool WaitingToStart::requiredNeighborCount() {
  byte neighborCount = 0;
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      neighborCount++;
    }
  }
  if (neighborCount >= MIN_NEIGHBOR_COUNT) {
    game->tileCount = neighborCount;
    return true;
  }
  return false;
}

static void WaitingToStart::countNeighbors(Game* game, byte face) {
  game->neighbors[face] = EXISTS;
}
    
bool WaitingToStart::initializeLeader() {
  game->isLeader = true;

  iterateNeighbors(WaitingToStart::countNeighbors);
}
