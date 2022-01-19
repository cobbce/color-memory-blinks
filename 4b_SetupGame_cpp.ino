void SetupGame::loopForState() {  
  if (game->isLeader) {
    setupLeader();
  } else {
    setupNeighbor();
  }
}

void SetupGame::broadcastCurrentState() {
  bool leaderWaitingForNeighbors = game->isLeader && !neighborsInSetupState;
  bool neighborWaitingForColorAssignment = !game->isLeader && !setupComplete;
  
  if (leaderWaitingForNeighbors || neighborWaitingForColorAssignment) {
    setValueSentOnAllFaces(state);
  }
}

////////////////
// Leader
////////////////

void SetupGame::setupLeader() {
  if (setupComplete) {
    checkButtonForStateChange(PLAY_GAME);
    return;
  }
    
  if (!neighborsAreInSetupState()) {
    return;
  }
  sendNeighborColors();
}

bool SetupGame::neighborsAreInSetupState() {
  if (neighborsInSetupState) {
    return true;
  }
  
  byte neighborsReadyCount = 0;
  
  FOREACH_FACE(face) {
    if (EMPTY == game->neighbors[face]) {
      continue;
    }
    
    if (getNeighborState(face) == SETUP_GAME) {
      neighborsInSetup[face] = 1;
      neighborsReadyCount++;
    }
  }
  
  if (neighborsReadyCount == REQUIRED_NEIGHBOR_COUNT) {
    neighborsInSetupState = true;
  }
  return neighborsInSetupState;
}

void SetupGame::sendNeighborColors() {
  byte neighborsAcknowledged = 0;
  
  FOREACH_FACE(face) {
    if (EMPTY == game->neighbors[face]) {
      continue;
    }

    // send color assignment to tile
    byte colorIndex = getColorIndex(face);
    setColorOnFace(getColorByIndex(colorIndex), face);
    setValueSentOnFace(colorIndex, face);

    // check if color assignment is confirmed
    if (getLastValueReceivedOnFace(face) == colorIndex) {
      neighborsInSetup[face] = 2;
      neighborsAcknowledged++;
    }
  }

  if (neighborsAcknowledged == REQUIRED_NEIGHBOR_COUNT) {
    setupComplete = true;
    debugPrint("setup complete for leader");
  }
}

////////////////
// Neighbor
////////////////

void SetupGame::setupNeighbor() {
  if (setupComplete) {
    return;
  }
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      byte neighborState = getNeighborState(face);

      // check for assigned color from leader
      if (neighborState >= COLOR_OFFSET) {
        setupComplete = true;
        
        color = getColorByIndex(neighborState);
        showColor();

        // send color index back to acknowledge
        setValueSentOnFace(neighborState, face);
      }
    }
  }
}
