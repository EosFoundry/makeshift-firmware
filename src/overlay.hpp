#ifndef OVERLAY_H_
#define OVERLAY_H_

#include <widget.hpp>

inline namespace mkshft_ui {
class Overlay : public Widget 
{
public:
  using Widget::Widget;
  
  uint32_t fadeTimeMs = 400;
};
} // namespace mkshft_ui

#endif
