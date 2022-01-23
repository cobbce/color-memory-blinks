
void PlayGame::runGame() {
  switch(leaderState) {
    case LEADER_SYNC_STATE:
      synchronizeState();
      break;
    case PAUSE_BEFORE_FIRST_SHOW_MOVE:
      pauseBeforeFirstShowMove();
      break;
    case SEND_SHOW_MOVE:
      sendShowMove();
      break;
    case WAIT_FOR_SHOW_MOVE_ACK:
      waitForShowMoveAck();
      break;
    case WAIT_FOR_SHOW_MOVE_DONE:
      waitForShowMoveDone();
      break;
    case PAUSING_BEFORE_NEXT_SHOW_MOVE:
      pauseBeforeShowingNextMove();
      break;
    case WAITING_FOR_PLAYER_MOVE:
      waitForPlayerMove();
      break;
    case ACK_PLAYER_MOVE:
      ackPlayerMove();
      break;
    case ENDING_GAME:
      endingGame();
      break;
    case END_GAME:
      endGame();
      break;
  }
}

void PlayGame::synchronizeState() {      
  byte neighborsInPlayGameState = 0;
  FOREACH_FACE(face) {        
    if (getNeighborState(face) == PLAY_GAME) {
      neighborsInPlayGameState += 1;
    }
  }

  if (neighborsInPlayGameState == REQUIRED_NEIGHBOR_COUNT) {
    setValueSentOnAllFaces(SYNC_STATE_DONE);
    leaderState = PAUSE_BEFORE_FIRST_SHOW_MOVE;
    timer.set(PLAY_GAME_DELAY);
  }
}

void PlayGame::pauseBeforeFirstShowMove() {
  if (timer.isExpired()) {
    leaderState = SEND_SHOW_MOVE;
  }
}

void PlayGame::sendShowMove() {
  currentTile = moves[currentShownMove];
  setValueSentOnFace(SHOW_MOVE_FLAG, currentTile);
  leaderState = WAIT_FOR_SHOW_MOVE_ACK;
}

void PlayGame::waitForShowMoveAck() {
  if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_FLAG) {
    setValueSentOnFace(PLAY_GAME, currentTile);
    leaderState = WAIT_FOR_SHOW_MOVE_DONE;
  }
}

void PlayGame::waitForShowMoveDone() {
  if (getLastValueReceivedOnFace(currentTile) == SHOW_MOVE_DONE_FLAG) {
    leaderState = PAUSING_BEFORE_NEXT_SHOW_MOVE;
    timer.set(PAUSE_BETWEEN_SHOWN_MOVES);
  }
}

void PlayGame::pauseBeforeShowingNextMove() {
  if (timer.isExpired()) {
    currentShownMove++;
    if (currentShownMove == MOVE_COUNT) {
      expectedPlayerMoveIndex = 0;
      leaderState = WAITING_FOR_PLAYER_MOVE;
      sendPlayerTurnStarted();
    } else {
      leaderState = SEND_SHOW_MOVE;
    }
    setColor(OFF);
  }
}
  
void PlayGame::waitForPlayerMove() {
  FOREACH_FACE(face) {
    if (getLastValueReceivedOnFace(face) == PLAYER_MOVED_FLAG) {
      Serial.print("clicked: "); Serial.println(face);
      startAckMove(face);
      if (face == moves[expectedPlayerMoveIndex]) {
        expectedPlayerMoveIndex++;
        if (expectedPlayerMoveIndex == MOVE_COUNT) {
          gameOver(true);
        }
      } else {
          gameOver(false);
      }
    }
  }
}

void PlayGame::ackPlayerMove() {
  if (!timer.isExpired()) {
    return;
  }
  setValueSentOnFace(PLAY_GAME, lastClickedFace);
  lastClickedFace = EMPTY;
  leaderState = WAITING_FOR_PLAYER_MOVE;
}

void PlayGame::endingGame() {
  if(timer.isExpired()) {
    if(game->isWinner) {
      setValueSentOnAllFaces(GAME_WON);
    }
    timer.set(200);
    leaderState = END_GAME;
  }
}

void PlayGame::endGame() {
  if(timer.isExpired()) {
    changeState(GAME_OVER);
  }
}

void PlayGame::sendPlayerTurnStarted() {
  setValueSentOnAllFaces(PLAYER_TURN_STARTED);
}

void PlayGame::gameOver(bool isWinner) {
  game->isWinner = isWinner;
  leaderState = ENDING_GAME;
  timer.set(100);
}

void PlayGame::startAckMove(byte face) {
  setValueSentOnFace(PLAYER_MOVED_ACK, face);
  timer.set(ACK_MOVE_DURATION);
  lastClickedFace = face;
  leaderState = ACK_PLAYER_MOVE;
}
