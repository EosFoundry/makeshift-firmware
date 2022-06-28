#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <widget.hpp>

inline namespace mkshft_ui {
class Overlay : public Widget {
public:
  using Widget::Widget;
  
  int fadeTimeMs = 400;
};
} // namespace mkshft_display

#endif