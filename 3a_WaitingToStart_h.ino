class WaitingToStart : public GameState {
  private:
    
  public:
    WaitingToStart(Game &game) : GameState(game, WAITING_TO_START, YELLOW, SETUP_GAME) {}
    
    void loopForState();
  private:
    bool requiredNeighborCount();
    bool initializeLeader();
};
