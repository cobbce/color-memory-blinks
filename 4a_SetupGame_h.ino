class SetupGame : public GameState {
  private:
    bool neighborsInSetupState;
    bool setupComplete;
    byte neighborsInSetup[FACE_COUNT];
  
  public:
    SetupGame(Game &game) : GameState(game, SETUP_GAME, MAGENTA, PLAY_GAME) {
      initState();
    }

    void initState() {
      neighborsInSetupState = false;
      setupComplete = false;
      
      FOREACH_FACE(f) {
        neighborsInSetup[f] = EMPTY;
      }
    }
    
    void loopForState();
    void broadcastCurrentState();
    
  private:
    void setupLeader();
    void setupNeighbor();
    bool neighborsAreInSetupState();
    void sendNeighborColors();
};
