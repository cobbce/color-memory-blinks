class GameOver : public GameState {
  public:
    GameOver(Game &game) : GameState(game, GAME_OVER, WAITING_TO_START) {
      init();
    }

    void init() {
      game->color = RED;
    }

    void loopForState() {
      checkButtonForStateChange(WAITING_TO_START);
    }
    
    virtual void broadcastCurrentState() {
      setValueSentOnAllFaces(state);
    }
};
