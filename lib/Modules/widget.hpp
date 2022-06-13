#ifndef WIDGET_H_
#define WIDGET_H_

#include <tgx.h>
#include <list>

inline namespace mkshft_display {
using namespace tgx;

extern Image<RGB565> *defaultCanvas;
void setDefaultCanvas(Image<RGB565> *cnv);

class Widget {
public:
  Widget()
      : parent(nullptr), canvas(defaultCanvas), root(iVec2(0, 0)),
        dimensions(iVec2(canvas->lx(), canvas->ly())) {
    _generateParameters();
  }
  Widget(iVec2 rt, iVec2 dmsn)
      : parent(nullptr), canvas(defaultCanvas), root(rt), dimensions(dmsn) {
    _generateParameters();
  }
  Widget(Image<RGB565> *cnv, iVec2 rt, iVec2 dmsn)
      : parent(nullptr), canvas(cnv), root(rt), dimensions(dmsn) {
    _generateParameters();
  }

  Widget(Widget const *parent)
      : parent(parent), canvas(parent->canvas) {
    // TODO: figure out a sane way to create and attach widgets to parents
    // Maybe build on single root render tree idea
  }

  Widget(Widget const *parent, iVec2 rt, iVec2 dmsn)
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

  void setMargin(uint16_t all);
  void setMargin(uint16_t vert, uint16_t horz);
  void setMargin(uint16_t top, uint16_t horz, uint16_t bottom);
  void setMargin(uint16_t top, uint16_t right, uint16_t bottom, uint16_t left);
  void setCornerRadius(int r);
  void setBorderWidth(int bw);
  // void setOpacity(float op) { this->opacity = op; }

  void render();
  void attach(Widget newChild);
  Box2<int> getBox();
  Widget const *parent;
  std::list<Widget> children;

protected:
  Image<RGB565> *const canvas;
  iVec2 root;
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
  void _renderSelf();
  void _renderChildren();
  void _generateParameters();
};

class Circle : public Widget {
public:
  Circle(iVec2 rt, int radius)
    : Widget(rt, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }
  Circle(Image<RGB565> *cnv, iVec2 rt, int radius)
      : Widget(cnv, rt, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }
};

class TextBox : public Widget {
    public:
    TextBox(Widget parent, String Text) : Widget(parent) {
      //asdf
    }
    protected:
    // overload for text
    void _renderSelf();
};
}
#endif // WIDGET_H_
