#ifndef MKSHFT_UI_H_
#define MKSHFT_UI_H_

#include <Arduino.h>

#include <map>
#include <string>
#include <vector>

#include <overlay.hpp>
#include <widget.hpp>

inline namespace mkshft_ui {

/**
 * Layout widget
 */
class Layout {
public:
  Layout(std::string id) : id(id){};

  std::map<std::string, Widget *> renderStack;
  std::map<std::string, WProgressBar> progressBars;
  std::map<std::string, WTextBox> textBoxes;
  std::map<std::string, WCircle> circles;
  std::map<std::string, WBox> boxes;

  std::string getId() { return id; };
  void addWidget(WidgetType t, std::string id);

protected:
  std::string id;
};

extern Layout *currentLayout;
extern std::map<std::string, Layout> layouts;

void init(Image<RGB565> *cnv);
void renderUI();
void testUI();
} // namespace mkshft_ui

#endif