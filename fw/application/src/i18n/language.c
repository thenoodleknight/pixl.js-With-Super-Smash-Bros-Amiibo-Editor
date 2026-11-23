#include "language.h"

// Only English strings are built into the firmware to save flash.
// The Language enum is kept so stored settings and other code keep working.
Language currentLanguage = LANGUAGE_EN_US;

const char *getLangString(L_StringID stringID) {
    if (stringID >= _L_COUNT) {
        return "@@STR@@";
    }
    return lang_en_US[stringID];
}

void setLanguage(Language lang) { currentLanguage = LANGUAGE_EN_US; }

const char *getLangDesc(Language lang) {
    if (lang == LANGUAGE_EN_US) {
        return "English";
    }
    return "@@LANG@@";
}

Language getLanguage() { return currentLanguage; }
