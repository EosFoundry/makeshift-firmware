#ifndef LAYOUT_H_
#define LAYOUT_H_

#include <widget.hpp>

inline namespace mkshft_ui {
class Layout : public Widget {
public:
  Layout() : Widget() {}
  Layout(Image<RGB565> *cnv)
      : Widget(cnv, iVec2(0, 0), iVec2(cnv->lx(), cnv->ly())) {}

  bool active = false;

protected:
  void _generateParameters();

};

} // namespace mkshft_display

#endif