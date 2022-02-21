#define FIRST_LEVEL 85
#define SECOND_LEVEL 170
#define SECOND_LEVEL 255

class GameOver : public GameState {
  public:
    GameOver(Game &game) : GameState(game, GAME_OVER, WAITING_TO_START) {
      init();
    }

    void init() override {
      setColor(getGameOverColor());
      game->timer.set(2000);
    }

    Color getGameOverColor() {
      return dim(getBaseColor(), 85 * (((game->turnCount - 1) % 3) + 1));
    }

    Color getBaseColor() {
      if (game->turnCount <= 3) {
        return RED;
      } else if (game->turnCount <= 6) {
        return ORANGE;
      } else if (game->turnCount <= 9) {
        return YELLOW;
      } else if (game->turnCount <= 12) {
        return GREEN;
      } else if (game->turnCount <= 15) {
        return BLUE;
      } else if (game->turnCount <= 18) {
        return MAGENTA;
      } else {
        return WHITE;
      }
    }

    void loopForState() override {
      if (game->timer.isExpired()) {
        checkButtonForStateChange(WAITING_TO_START);
      }
    }
    
    void broadcastCurrentState() override {
      setValueSentOnAllFaces(state);
    }
};
