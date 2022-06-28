#include <mkshft_ui.hpp>

inline namespace mkshft_ui {
void init() {
  overlayOn = false;
  sinceOverlayShown = 0;
}


void checkOverlay() {
  if (overlayOn == false) {
    sinceOverlayShown = 0;
  } else { // check against the overlay's individual internal fade time
    if (overlays[currentOverlayKey].fadeTimeMs < sinceOverlayShown) {
      overlayOn = false;
      sinceOverlayShown = 0;
    }
  }
}

void renderUI() {
  currentLayout->second.render();
  if (overlayOn) {
    overlays[currentOverlayKey].render();
  }
}
} // namespace mkshft_ui