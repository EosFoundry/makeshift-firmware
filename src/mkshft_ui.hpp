#ifndef MKSHFT_UI_H_
#define MKSHFT_UI_H_

#include <layout.hpp>
#include <map>
#include <overlay.hpp>
#include <string.h>
#include <widget.hpp>

inline namespace mkshft_ui {

typedef std::map<std::string, Widget>::iterator WidgetIter;
typedef std::map<std::string, Layout>::iterator LayoutIter;

extern WidgetIter currentLayout;
extern std::string currentOverlayKey;

extern std::map<std::string, Overlay> overlays;
extern std::map<std::string, Layout> layouts;

extern elapsedMillis sinceOverlayShown;

extern bool overlayOn;

void showOverlay(std::string overlayKey);
void checkOverlay();
void renderUI();
} // namespace mkshft_ui

#endif