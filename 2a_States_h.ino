class PlayGame : public GameState {
  public:
    PlayGame(Game &game) : GameState(game, PLAY_GAME, GREEN, GAME_OVER) {}

    void loopForState();
};

class GameOver : public GameState {
  public:
    GameOver(Game &game) : GameState(game, GAME_OVER, RED, WAITING_TO_START) {}

    void loopForState();
};
