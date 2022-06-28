#ifndef WIDGET_H_
#define WIDGET_H_

#include <list>
#include <math.h>
#include <tgx.h>

inline namespace mkshft_ui {
using namespace tgx;

extern Image<RGB565> *defaultCanvas;
/**
 * Base class for all widgets, implements a rectangular bounding box and
 * CSS-style margin/padding/border properties
 */
class Widget {
public:
  Widget()
      : parent(nullptr), canvas(defaultCanvas), anchor(iVec2(0, 0)),
        dimensions(iVec2(canvas->lx(), canvas->ly())) {
    _generateParameters();
  }
  Widget(iVec2 anchor, iVec2 dmsn)
      : parent(nullptr), canvas(defaultCanvas), anchor(anchor),
        dimensions(dmsn) {
    _generateParameters();
  }
  Widget(Image<RGB565> *cnv, iVec2 anchor, iVec2 dmsn)
      : parent(nullptr), canvas(cnv), anchor(anchor), dimensions(dmsn) {
    _generateParameters();
  }

  Widget(Widget const *parent) : parent(parent), canvas(parent->canvas) {
    // TODO: figure out a sane way to create and attach widgets to parents
    // Maybe build on single root render tree idea
    _generateParametersFromParent();
    for (int n = 0; n < 4; n++) {
      margin[n] = parent->margin[n];
      padding[n] = parent->padding[n];
    }
  }

  Widget(Widget const *parent, iVec2 anchor, iVec2 dmsn)
      : parent(parent), canvas(parent->canvas) {
    for (int n = 0; n < 4; n++) {
      margin[n] = parent->margin[n];
      padding[n] = parent->padding[n];
    }
  }

  void setFillColor(RGB32 fill) { fillColor = fill; };
  void setBorderColor(RGB32 border) { borderColor = border; };
  void setColors(RGB32 fill, RGB32 border) {
    fillColor = fill;
    borderColor = border;
  }

  virtual void setSize(iBox2 newBox);
  void setMargin(uint16_t all);
  void setMargin(uint16_t vert, uint16_t horz);
  void setMargin(uint16_t top, uint16_t horz, uint16_t bottom);
  void setMargin(uint16_t top, uint16_t right, uint16_t bottom, uint16_t left);
  void setCornerRadius(int r);
  void setBorderWidth(int bw);
  void setOpacity(float op) { this->opacity = op; }

  void render();
  void addChild(Widget *newChild);
  void setParent(Widget const *newParent) { parent = newParent; };
  Box2<int> getBox();

  Widget const *parent;
  std::list<Widget *> children;

protected:
  Image<RGB565> *const canvas;
  iVec2 anchor;
  iVec2 center;
  iVec2 dimensions;
  uint16_t padding[4];
  uint16_t margin[4];
  RGB32 fillColor;

  int borderWidth;
  RGB32 borderColor;

  float opacity;
  // int is used here due to tgx bug
  int cornerRadius;
  int borderRadius;
  iBox2 box;
  iBox2 borderBox;

  virtual void _renderSelf();
  virtual void _renderChildren();
  virtual void _generateParameters();
  virtual void _generateParametersFromParent();
};

/**
 * Circle widget, hacks with default widget rendering
 * 
 * TODO: convert to use tgx::fillCircle functions
 */
class Circle : public Widget {
public:
  Circle(iVec2 center, int radius) : Widget(center, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }
  Circle(Image<RGB565> *cnv, iVec2 anchor, int radius)
      : Widget(cnv, anchor, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }
};

/**
 * Fundamental text rendering widget, rendering uses tgx character width 
 * calculations to allow auto clipping and wrapping.
 *
 * TODO: word wrap instead of simple character wrapping
 */
class TextBox : public Widget {
public:
  TextBox(Widget parent, String Text) : Widget(parent) {

  }

  bool clip;
  bool wrap;

protected:
  // overload for text
  void _renderSelf() override;
};

/**
 * Loading Bar widget which adjusts progress automatically according to width.
 * Fill directions are currently coded for down -> up, and left -> right.
 * 
 * TODO: figure out now to efficiently code for bars that reverse direction.
 */
class LoadingBar : public Widget {
public:
  enum FILL_DIRECTION { UP, DOWN, LEFT, RIGHT };

  LoadingBar() : Widget(), fillDirection(RIGHT) { setSize(box); };
  LoadingBar(Widget const *parent) : Widget(parent), fillDirection(RIGHT) {
    Serial.println("Creating loadingbar");
    setSize(box);
    setFillDirection(fillDirection);
    setProgress(0);
  };

  void setFillDirection(FILL_DIRECTION newFD);
  void setSize(iBox2 newBox) override;
  void setProgress(int prog);
  void setBackgroundColor(RGB32 background) { backgroundColor = background; }
  void setColors(RGB32 fill, RGB32 background, RGB32 border) {
    fillColor = fill;
    borderColor = border;
    backgroundColor = background;
  }

protected:
  RGB32 backgroundColor;
  int progressBoxMax;
  uint8_t progress;
  FILL_DIRECTION fillDirection;
  float fProgressRatio;
  iBox2 progressBox;

  void _renderSelf() override;
  // void _generateParameters() override;
};

/**
 * Helper functions
 */

void setDefaultCanvas(Image<RGB565> *cnv);
void link(Widget *parent, Widget *child);

} // namespace mkshft_ui
#endif // WIDGET_H_
