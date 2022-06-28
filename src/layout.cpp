#include <layout.hpp>

inline namespace mkshft_ui {
void Layout::_generateParameters() {
  Serial.println("Generating parameters for Layout...");
  int minx = anchor.x;
  int maxx = anchor.x + dimensions.x;
  int miny = anchor.y;
  int maxy = anchor.y + dimensions.y;
  box = iBox2(minx, maxx, miny, maxy);
  borderBox = iBox2(box);
  center = box.center();

  cornerRadius = 0;
  borderRadius = 0;
  borderWidth = 0;

  opacity = 0.0f;

  fillColor = RGB32(0, 0, 0);
  borderColor = RGB32(0, 0, 0);

  active = false;
}
} // namespace mkshft_ui