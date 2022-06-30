#include <widget.hpp>

inline namespace mkshft_ui {

Image<RGB565> *defaultCanvas = nullptr;

void _printBox(iBox2 b) {
  Serial.print("min: (");
  Serial.print(b.minX);
  Serial.print(",");
  Serial.print(b.minY);
  Serial.print(") | max: (");
  Serial.print(b.maxX);
  Serial.print(",");
  Serial.print(b.maxY);
  Serial.print(")");
}

void Widget::setSize(iBox2 newBox) {
  box = iBox2(newBox);
  center = box.center();
  dimensions = iVec2(box.lx(), box.ly());

  // this generates new border box
  setBorderWidth(borderWidth);
}

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

void Widget::addChild(Widget *newChild) { children.push_back(newChild); }

void Widget::_renderSelf() {
  if (borderWidth > 0) {
    canvas->fillRoundRect(borderBox, borderRadius, borderColor, opacity);
  }
  canvas->fillRoundRect(box, cornerRadius, fillColor, opacity);
}

void Widget::_renderChildren() {
  for (auto &child : children) {
    Serial.print("rendering child box: ");
    _printBox(child->box);
    child->render();
  }
}
void Widget::_generateParameters() {
  int minx = anchor.x;
  int maxx = anchor.x + dimensions.x;
  int miny = anchor.y;
  int maxy = anchor.y + dimensions.y;
  cornerRadius = 0;
  box = iBox2(minx, maxx, miny, maxy);
  center = box.center();
  fillColor = RGB32(0, 0, 0);
  borderColor = RGB32(0, 0, 0);
  borderRadius = 0;
  borderWidth = 0;
  opacity = 1.0f;
  borderBox = iBox2(box);
  Serial.println("Generating parameters for widget: ");
  _printBox(iBox2(anchor.x, dimensions.x, anchor.y, dimensions.y));
  Serial.println();
}

void Widget::_generateParametersFromParent() {
  anchor = parent->anchor;
  dimensions = parent->dimensions;
  cornerRadius = 0;
  box = iBox2(parent->box);
  center = box.center();
  fillColor = RGB32(0, 0, 0);
  borderColor = RGB32(0, 0, 0);
  borderRadius = 0;
  borderWidth = 0;
  opacity = 1.0f;
  borderBox = iBox2(box);
  Serial.println("Generating parameters for widget: ");
  _printBox(iBox2(anchor.x, dimensions.x, anchor.y, dimensions.y));
  Serial.println();
}

void TextBox::_renderSelf() {
  if (borderWidth > 0) {
    canvas->fillRoundRect(borderBox, borderRadius, borderColor, opacity);
  }
  canvas->fillRoundRect(box, cornerRadius, fillColor, opacity);
  // canvas->drawChar('c', iVec1(0,0), fillColor,  GFXfont);
}

void LoadingBar::setFillDirection(FILL_DIRECTION newFD) {
  if (fillDirection != newFD) {
    RGB32 tempFill = fillColor;
    fillColor = RGB32_Transparent;
    // reset progressBox to max size
    setSize(box);
    switch (newFD) {
    case UP:
      progressBoxMax = progressBox.ly();
      break;
    case RIGHT:
      progressBoxMax = progressBox.lx();
      break;
    default:
      // do nothing for now
      break;
    }
    setProgress(progress);
    fillDirection = newFD;
    fillColor = tempFill;
  }
}

void LoadingBar::setSize(iBox2 newBox) {
  box = iBox2(newBox);
  progressBox = iBox2(box);

  progressBox.minX += 8;
  progressBox.maxX -= 8;
  progressBox.minY += 8;
  progressBox.maxY -= 8;
  Serial.print("Setting progress box: ");
  _printBox(progressBox);
  Serial.println();

  switch (fillDirection) {
  case RIGHT:
  case LEFT:
    fProgressRatio = (float)progressBox.lx() / 100.0f;
    break;
  case UP:
  case DOWN:
    fProgressRatio = (float)progressBox.ly() / 100.0f;
    break;
  default:
    break;
  }
}

void LoadingBar::setProgress(int prog) {
  if (prog >= 100) {
    progress = 100;
  } else if (prog < 0) {
    progress = 0;
  } else {
    progress = prog;
  }

  Serial.print("Setting progress: ");
  Serial.println(progress);

  float fProgGap = (float)progress * fProgressRatio;
  int iProgGap = round(fProgGap);

  switch (fillDirection) {
  case RIGHT:
    progressBox.maxX = progressBox.minX + iProgGap;
    break;
  case UP:
    progressBox.minY = progressBox.maxY - iProgGap;
    break;
  default:
    break;
  }
  Serial.println('c');
}

void LoadingBar::_renderSelf() {
  // if (borderWidth > 0) {
  //   canvas->fillRoundRect(borderBox, borderRadius, borderColor, opacity);
  // }
  // canvas->fillRoundRect(box, cornerRadius, backgroundColor, opacity);
  canvas->fillRect(progressBox, backgroundColor);

  Serial.print("Rendering prog bar with box: ");
  _printBox(progressBox);
  Serial.println();

  // canvas->fillRoundRect(progressBox, cornerRadius, fillColor, opacity);
  _renderChildren();
}

void setDefaultCanvas(Image<RGB565> *cnv) { defaultCanvas = cnv; }

void link(Widget *parent, Widget *child) {
  bool addChild = true;
  for (auto &existingChild : parent->children) {
    if (existingChild == child) {
      addChild = false;
    }
  }
  if (addChild == true) {
    parent->addChild(child);
  }

  if (child->parent != parent) {
    child->setParent(parent);
  }
}

} // namespace mkshft_display
