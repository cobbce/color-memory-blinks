void WaitingToStart::loopForState() {
  if (buttonSingleClicked()) {
    if (requiredNeighborCount()) {
      debugPrint("Starting game");
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
    
bool WaitingToStart::initializeLeader() {
  game->isLeader = true;
  
  FOREACH_FACE(face) {
    if (hasNeighbor(face)) {
      game->neighbors[face] = face;
    }
  }
}
