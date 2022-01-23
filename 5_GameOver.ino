class GameOver : public GameState {
  public:
    GameOver(Game &game) : GameState(game, GAME_OVER, WAITING_TO_START) {
      init();
    }

    void init() override {
      if (game->isWinner) {
        setColor(GREEN);
      } else {
        setColor(RED);
      }
    }

    void loopForState() override {
      checkButtonForStateChange(WAITING_TO_START);
    }
    
    void broadcastCurrentState() override {
      setValueSentOnAllFaces(state);
    }
};
