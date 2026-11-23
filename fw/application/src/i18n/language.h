#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "string_id.h"

#define _T(x) getLangString(_L_##x)


typedef enum {
    LANGUAGE_ZH_HANS,
    LANGUAGE_EN_US,
    LANGUAGE_ZH_TW,
    LANGUAGE_ES_ES,
    LANGUAGE_HU_HU,
    LANGUAGE_DE_DE,
    LANGUAGE_FR_FR,
    LANGUAGE_NL_NL,
    LANGUAGE_PT_BR,
    LANGUAGE_JA_JP,
    LANGUAGE_PT_PT,
    LANGUAGE_IT_IT,
    LANGUAGE_RU_RU,
    LANGUAGE_PL_PL,
    LANGUAGE_SV_SE,
    LANGUAGE_COUNT
} Language;

extern const char* lang_en_US[_L_COUNT];

// 获取字符串的函数 (Get language string function)
const char* getLangString(L_StringID stringID);
void setLanguage(Language lang);
Language getLanguage();
const char* getLangDesc(Language lang);


#endif // LANGUAGE_H
