#include <widget.hpp>

inline namespace mkshft_display {

Image<RGB565> *defaultCanvas = nullptr;

void setDefaultCanvas(Image<RGB565> *cnv) { defaultCanvas = cnv; }

void Widget::setMargin(uint16_t all) { setMargin(all, all, all, all); }

void Widget::setMargin(uint16_t vert, uint16_t horz) {
  setMargin(vert, horz, vert, horz);
}

void Widget::setMargin(uint16_t top, uint16_t horz, uint16_t bottom) {
  setMargin(top, horz, bottom, horz);
}

void Widget::setMargin(uint16_t top, uint16_t right, uint16_t bottom,
                       uint16_t left) {
  margin[0] = top;
  margin[1] = right;
  margin[2] = bottom;
  margin[3] = left;
}

void Widget::setBorderWidth(int bw) {
  borderWidth = bw;
  borderBox = iBox2(box);
  borderBox.minX -= borderWidth;
  borderBox.minY -= borderWidth;
  borderBox.maxX += borderWidth;
  borderBox.maxY += borderWidth;
}

void Widget::setCornerRadius(int r) {
  cornerRadius = r;
  if (cornerRadius > 0) {
    borderRadius = cornerRadius + borderWidth;
  }
}

void Widget::render() {
  _renderSelf();
  _renderChildren();
}

void Widget::_renderSelf() {
  if (borderWidth > 0) {
    canvas->fillRoundRect(borderBox, borderRadius, borderColor, opacity);
  }
  canvas->fillRoundRect(box, cornerRadius, fillColor, opacity);
}

void Widget::_renderChildren() {
  for (auto &child : children) {
    child.render();
  }
}
void Widget::_generateParameters() {
  int minx = root.x;
  int maxx = root.x + dimensions.x;
  int miny = root.y;
  int maxy = root.y + dimensions.y;
  cornerRadius = 0;
  box = iBox2(minx, maxx, miny, maxy);
  center = box.center();
  fillColor = RGB32(0, 0, 0);
  borderColor = RGB32(0, 0, 0);
  borderRadius = 0;
  borderWidth = 0;
  opacity = 1.0f;
  borderBox = iBox2(box);
}

void TextBox::_renderSelf() {
  // if (borderWidth > 0) {
  //   canvas->fillRoundRect(borderBox, borderRadius, borderColor, opacity);
  // }
  // canvas->fillRoundRect(box, cornerRadius, fillColor, opacity);
  // TODO: finish implementing fonts
  canvas->drawChar('c', iVec2(0,0), fillColor,  GFXfont);
  // Render text constrained to box
}

} // namespace mkshft_display
