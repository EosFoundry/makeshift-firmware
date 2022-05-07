#include <color.hpp>
// #define DEBUG 1

ColorDelta operator-(const Color a, const Color b) {
    ColorDelta del;
    del.r = a.r - b.r;
    del.g = a.g - b.g;
    del.b = a.b - b.b;
    return del;
}

ColorSequence createFadeSequence(uint32_t frames, Color start, Color end) {
  return createFadeSequence(frames, start, end, true, false);
}

ColorSequence createFadeSequence(uint32_t frames, Color start, Color end,
                                 bool adjusted, bool isLooping) {
  ColorSequence sequence;
  std::list<ColorEvent> newQ;
  float rFlt, gFlt, bFlt;
  uint8_t arr, gee, bee, prevR, prevG, prevB;
  float ratioR, ratioG, ratioB;
  ColorDelta delta = end - start;
  newQ.clear();
#ifdef DEBUG
  printColor(delta);
  Serial.println();
#endif

  rFlt = (float)start.r;
  gFlt = (float)start.g;
  bFlt = (float)start.b;

  ratioR = (float)delta.r / frames;
  ratioG = (float)delta.g / frames;
  ratioB = (float)delta.b / frames;

#ifdef DEBUG
  Serial.print("float ratios: ");
  Serial.print(ratioR);
  Serial.print(",");
  Serial.print(ratioG);
  Serial.print(",");
  Serial.print(ratioB);
  Serial.println();
#endif

  newQ.push_front({start, 1, adjusted});
  uint32_t eventFrames = 1;
  prevR = start.r;
  prevG = start.g;
  prevB = start.b;
  while (frames != 0) {
    rFlt = rFlt + ratioR;
    gFlt = gFlt + ratioG;
    bFlt = bFlt + ratioB;
    arr = (uint8_t)round(rFlt);
    gee = (uint8_t)round(gFlt);
    bee = (uint8_t)round(bFlt);

    if (arr == prevR && gee == prevG && bee == prevB) {
      eventFrames++;
    } else {
#ifdef DEBUG
      printColor((Color){arr, gee, bee});
      Serial.print(" | frames: ");
      Serial.print(eventFrames);
      Serial.println();
      #endif
      newQ.push_back({{prevR, prevG, prevB}, eventFrames, adjusted});
      eventFrames = 1;
    }
    prevR = arr;
    prevG = gee;
    prevB = bee;
    frames --;
  }
  newQ.push_back({end, 1, adjusted});
  sequence.events = newQ;
  sequence.loop = isLooping;
  return sequence;
}

// void joinSequences(ColorSequence startSeq, ColorSequence endSeq) {
//   ColorEvent evt;
//   while (endSeq.events.empty() == false) {
//     evt = endSeq.events.front();
//     startSeq.events.push_back();
//   }
// }

void setSequenceEmpty(ColorSequence * seq) {
    seq->events.clear();
    seq->events.push_front(EventOFF);
    seq->loop = true;
}

void printColor(ColorDelta d) {
  Serial.print(d.r);
  Serial.print(",");
  Serial.print(d.g);
  Serial.print(",");
  Serial.print(d.b);
}

void printColor(Color c) {
  Serial.print(c.r);
  Serial.print(",");
  Serial.print(c.g);
  Serial.print(",");
  Serial.print(c.b);
}

void printEvent(ColorEvent e) {
  Serial.print("{");
  printColor(e.color);
  Serial.print("|");
  Serial.print(e.lengthFrames);
  Serial.print("|");
  Serial.print(e.adjusted);
  Serial.print("}");
}

void printSequence(ColorSequence s) {
  Serial.println("Printing sequence... ");
  Serial.print("looping: ");
  Serial.println(s.loop);
  for (ColorSequenceItr currentEvent = s.events.begin();
       currentEvent != s.events.end(); currentEvent++) {
    printEvent(*currentEvent);
    Serial.println();
  }
}
