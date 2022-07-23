#ifndef WIDGET_H_
#define WIDGET_H_

#include <Arduino.h>
#include <list>
#include <math.h>
#include <string>
#include <tgx.h>
#include <vector>

#include <fonts.hpp>

#define DEBUG

inline namespace mkshft_ui {
using namespace tgx;

enum WidgetType {
  W_BOX,
  W_TEXT_BOX,
  W_CIRCLE,
  W_PROGRESS_BAR,
};

extern Image<RGB565> *defaultCanvas;
/**
 * Base class for all widgets, implements a rectangular bounding box and
 * CSS-style margin/padding/border properties
 */
class Widget {
public:
  Widget(std::string id)
      : id(id), canvas(defaultCanvas), anchor(iVec2(0, 0)),
        dimensions(iVec2(0, 0)) {
    _generateParameters();
  }
  Widget(std::string id, iVec2 anchor, iVec2 dmsn)
      : id(id), canvas(defaultCanvas), anchor(anchor), dimensions(dmsn) {
    _generateParameters();
  }
  Widget(std::string id, Image<RGB565> *cnv, iVec2 anchor, iVec2 dmsn)
      : id(id), canvas(cnv), anchor(anchor), dimensions(dmsn) {
    _generateParameters();
  }

  std::string getID() { return id; }
  void setFillColor(RGB32 fill) { fillColor = fill; };
  void setBorderColor(RGB32 border) { borderColor = border; };
  void setColors(RGB32 fill, RGB32 border) {
    fillColor = fill;
    borderColor = border;
  }

  virtual void setAnchor(uint16_t x, uint16_t y);
  virtual void setSize(uint16_t lx, uint16_t ly);
  void setMargin(uint16_t all);
  void setMargin(uint16_t vert, uint16_t horz);
  void setMargin(uint16_t top, uint16_t horz, uint16_t bottom);
  void setMargin(uint16_t top, uint16_t right, uint16_t bottom, uint16_t left);
  void setCornerRadius(int r);
  void setBorderWidth(int bw);
  void setOpacity(float op) { this->opacity = op; }

  virtual void render();

  Box2<int> getBox();

protected:
  static const WidgetType type;
  std::string id;
  uint16_t childCount = 0;
  Image<RGB565> *canvas;
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

  virtual void _generateParameters();
  virtual void _generateExtraParameters() = 0;
};

class WBox : public Widget {
  using Widget::Widget;

  static const WidgetType type;

protected:
  void _generateExtraParameters(){};
};
/**
 * Circle widget, hacks with default widget rendering
 *
 * TODO: convert to use tgx::fillCircle functions
 */
class WCircle : public Widget {
public:
  WCircle(std::string id) : Widget(id, iVec2(0, 0), iVec2(0, 0)) {
    setCornerRadius(0);
  }

  WCircle(std::string id, iVec2 center, int radius)
      : Widget(id, center, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }
  WCircle(std::string id, Image<RGB565> *cnv, iVec2 anchor, int radius)
      : Widget(id, cnv, anchor, iVec2(radius, radius)) {
    setCornerRadius(radius);
  }

  static const WidgetType type;

protected:
  void _generateExtraParameters(){};
};

/**
 * Fundamental text rendering widget, rendering uses tgx character width
 * calculations to allow auto clipping and wrapping.
 *
 * TODO: word wrap instead of simple character wrapping
 */
class WTextBox : public Widget {
public:
  WTextBox(std::string id) : Widget(id) {
    _generateExtraParameters();
    contents = "";
    contentByLine.push_back("");
  }
  WTextBox(std::string id, std::string text) : Widget(id) {
    _generateExtraParameters();
    contents = text;
    refitText();
  }

  static const WidgetType type;
  bool clip;
  bool wrap;
  iVec2 fontSz;
  std::string contents;
  std::vector<std::string> contentByLine;

  void setSize(uint16_t lx, uint16_t ly) override;
  /**
   * Sets text without refitting. Checks only if the text fits on a single line
   * and does nothing if it cannot fit into a single line.
   */
  void fastSetText(std::string txt);
  void setText(std::string txt);
  void refitText();
  void render() override;

protected:
  const GFXfont *fontFace;
  int maxLines = 1;
  int maxCharsInLine = 1;

  // overload for text
  void _generateExtraParameters() override;
};

/**
 * Loading Bar widget which adjusts progress automatically according to width.
 * Fill directions are currently coded for down -> up, and left -> right.
 *
 * TODO: figure out now to efficiently code for bars that reverse direction.
 */
class WProgressBar : public Widget {
public:
  enum FILL_DIRECTION { UP, DOWN, LEFT, RIGHT };

  WProgressBar(std::string id) : Widget(id), fillDirection(RIGHT) {
    setSize(box.lx(), box.ly());
    setFillDirection(fillDirection);
    setProgress(0);
  };

  static const WidgetType type;

  void setFillDirection(FILL_DIRECTION newFD);
  void setSize(uint16_t lx, uint16_t ly) override;
  void setProgress(int prog);
  void setBackgroundColor(RGB32 background) { backgroundColor = background; }
  void setColors(RGB32 fill, RGB32 background, RGB32 border) {
    fillColor = fill;
    borderColor = border;
    backgroundColor = background;
  }

  void render() override;

protected:
  RGB32 backgroundColor;
  int progressBoxMax;
  uint8_t progress;
  FILL_DIRECTION fillDirection;
  float fProgressRatio;
  iBox2 progressBox;

  // void _generateParameters() override;
  void _generateExtraParameters(){};
};

/**
 * Helper functions
 */

/**
 * Retargets the canvas, there is no way to gracefully switch to a new canvas
 * right now, this may be removed later
 */
void setDefaultCanvas(Image<RGB565> *cnv);

/**
 * Removes all control characters except for newline '\n'
 * formating space characters (e.g. \t \v) are also removed
 */
std::string removeControlChars(std::string txt);

} // namespace mkshft_ui
#endif // WIDGET_H_
