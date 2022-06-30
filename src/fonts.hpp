#ifndef FONTS_H_
#define FONTS_H_

#include <Arduino.h>
#include <tgx.h>
#include <map>
#include <string>

#include <fonts/iosevka-mkshft-thin8pt7b.h>
#include <fonts/iosevka-mkshft-bold8pt7b.h>

#include <fonts/iosevka-mkshft-regular9pt7b.h>
#include <fonts/iosevka-mkshft-thin7pt7b.h>

inline namespace fonts {

#define defFont(STYLE) iosevka_mkshft_##STYLE##pt7b

struct Font {
  GFXfont regular[3];
  GFXfont bold[3];
  GFXfont oblique[3];
};

const Font toaster = {
    {defFont(thin7), defFont(thin8), defFont(regular9)},
    {defFont(bold8), defFont(bold8), defFont(thin8)},
    {defFont(thin7), defFont(thin8), defFont(thin8)},
};

} // namespace fonts

#endif // FONTS_H_
