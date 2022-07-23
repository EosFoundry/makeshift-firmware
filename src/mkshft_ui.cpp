#include <mkshft_ui.hpp>

inline namespace mkshft_ui {

Layout *currentLayout;
std::map<std::string, Layout> layouts;

void Layout::addWidget(WidgetType t, std::string id) {
  bool emplaced = false;
  Widget *widg;

  std::string target;

#ifdef DEBUG
#endif
  switch (t) {
  case W_BOX: {
    auto e = currentLayout->boxes.emplace(id, WBox(id));
    widg = &e.first->second;
    emplaced = e.second;

    target = "box";

    break;
  }
  case W_CIRCLE: {
    auto e = currentLayout->circles.emplace(id, WCircle(id));
    widg = &e.first->second;
    emplaced = e.second;

    target = "circle";

    break;
  }
  case W_TEXT_BOX: {
    auto e = currentLayout->textBoxes.emplace(id, WTextBox(id));
    widg = &e.first->second;
    emplaced = e.second;

    target = "textbox";

    break;
  }
  case W_PROGRESS_BAR: {
    auto e = currentLayout->progressBars.emplace(id, WProgressBar(id));
    widg = &e.first->second;
    emplaced = e.second;

    target = "progress bar";

    break;
  }
  default: {
  }
  }

  if (emplaced == true) {
#ifdef DEBUG
    Serial.print("Created ");
    Serial.print(target.data());
    Serial.print(" with id \"");
    Serial.print(id.data());
    Serial.print("\" in layout \"");
    Serial.print(currentLayout->getId().data());
    Serial.print("\"");
    Serial.println();
#endif
    renderStack.insert(std::make_pair(id, widg));
  }
}
void init(Image<RGB565> *cnv) {

  setDefaultCanvas(cnv);

  layouts.emplace("default", Layout("default"));
  currentLayout = &layouts.at("default");

  
  // testUI();
}


void renderUI() {
  // Widget *renderTarget;
  // std::string label;
  // __pair : [first, second] maps to [label, widget]
  auto __pair = currentLayout->renderStack.begin();
  auto __end = currentLayout->renderStack.end();
  for (; __pair != __end; ++__pair) {
    // Serial.print("rendering ");
    // Serial.println(__pair->first.data());

    __pair->second->render();
  }
}

void testUI() {
  currentLayout->addWidget(W_BOX, "testBox");
  currentLayout->boxes.at("testBox").setSize(120, 120);
  currentLayout->boxes.at("testBox").setColors(tgx::RGB32_Purple,
                                               tgx::RGB32_Blue);
  currentLayout->addWidget(W_BOX, "testBox2");
  currentLayout->boxes.at("testBox2").setSize(12, 12);
  currentLayout->boxes.at("testBox2")
      .setColors(tgx::RGB32_Green, tgx::RGB32_Blue);

  std::string textName = "testText";
  currentLayout->addWidget(W_TEXT_BOX, textName);
  currentLayout->textBoxes.at(textName).setSize(320, 240);
  currentLayout->textBoxes.at(textName)
      .setColors(tgx::RGB32_White, tgx::RGB32_Blue);
  

  tgx::iBox2 boxxx = tgx::iBox2(0,0,0,0);
  int xadv = 0;
  std::string testString = "beegin..";
  for (char c = 20; c < 128; ++c) {
    boxxx = defaultCanvas->measureChar(c, iVec2(25, 25), *baseFont, &xadv);
    // Serial.print("Size of baseFont char \'");
    // Serial.print(c);
    // Serial.print("\': ");
    // Serial.print(boxxx.lx());
    // Serial.print(" x ");
    // Serial.print(boxxx.ly());
    // Serial.print(" | xadv: ");
    // Serial.print(xadv);
    // Serial.println();
    testString += c;
  }

  Serial.print("testString:");
  Serial.println(testString.data());
  currentLayout->textBoxes.at(textName).setText(testString);

  // layouts.at("default").
}
} // namespace mkshft_ui