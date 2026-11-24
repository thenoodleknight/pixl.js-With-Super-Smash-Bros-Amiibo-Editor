#include "smash_data.h"

#include <stdio.h>

#define MAIN_U16(n, o, mx) {n, o, 0, 16, SMASH_FIELD_UNSIGNED, SMASH_GROUP_MAIN, 0, mx}
#define MAIN_S16(n, o, mn, mx) {n, o, 0, 16, SMASH_FIELD_SIGNED, SMASH_GROUP_MAIN, mn, mx}
#define MAIN_ENUM(n, o, b, l) {n, o, b, l, SMASH_FIELD_ENUM, SMASH_GROUP_MAIN, 0, (1 << (l)) - 1}
#define MAIN_BOOL(n, o, b) {n, o, b, 1, SMASH_FIELD_BOOL, SMASH_GROUP_MAIN, 0, 1}
#define MAIN_ABILITY(n, o) {n, o, 0, 8, SMASH_FIELD_ABILITY, SMASH_GROUP_MAIN, 0, 255}
#define MII_ENUM(n, o, b, l) {n, o, b, l, SMASH_FIELD_ENUM, SMASH_GROUP_MII, 0, (1 << (l)) - 1}
#define PCT(n, o, b, l) {n, o, b, l, SMASH_FIELD_PERCENT, SMASH_GROUP_PERSONALITY, 0, (1 << (l)) - 1}

// Offsets/bit positions come from regions.json
const smash_field_t smash_fields[] = {
    MAIN_U16("Level EXP", 0x14C, 65535),
    MAIN_U16("CPU EXP", 0x14E, 65535),
    MAIN_S16("Attack", 0x150, -32768, 32767),
    MAIN_S16("Defense", 0x152, -32768, 32767),
    MAIN_ABILITY("Ability 1", 0xEC),
    MAIN_ABILITY("Ability 2", 0xED),
    MAIN_ABILITY("Ability 3", 0xEE),
    MAIN_BOOL("Learning", 0xE2, 7),
    MAIN_BOOL("Journey", 0xE2, 6),
    MAIN_ENUM("Spirit Type", 0xE3, 0, 2),
    MAIN_ENUM("Alternate Skin", 0x1A3, 4, 4),

    MII_ENUM("Hat", 0xF0, 0, 8),
    MII_ENUM("Outfit", 0xF2, 0, 8),
    MII_ENUM("Neutral Special", 0xEF, 6, 2),
    MII_ENUM("Side Special", 0xEF, 4, 2),
    MII_ENUM("Up Special", 0xEF, 2, 2),
    MII_ENUM("Down Special", 0xEF, 0, 2),
    MII_ENUM("Voice/Starter Pokemon", 0x1A2, 0, 8),

    PCT("Near", 0x168, 7, 7),
    PCT("Offensive", 0x168, 0, 7),
    PCT("Grounded", 0x169, 1, 7),
    PCT("Attack Out Cliff", 0x16A, 2, 6),
    PCT("Dash", 0x16B, 4, 7),
    PCT("Return To Cliff", 0x16C, 5, 6),
    PCT("Air Offensive", 0x16D, 7, 6),
    PCT("Cliffer", 0x16D, 1, 6),
    PCT("Feint Master", 0x16E, 3, 7),
    PCT("Feint Counter", 0x16F, 4, 7),
    PCT("Feint Shooter", 0x170, 5, 7),
    PCT("Grab", 0x171, 6, 7),
    PCT("100 Attacker", 0x172, 7, 6),
    PCT("100 Keeper", 0x172, 1, 6),
    PCT("Attack Cancel", 0x173, 3, 6),
    PCT("Smash Holder", 0x174, 5, 7),
    PCT("Dash Attack", 0x175, 6, 7),
    PCT("Critical Hitter", 0x176, 7, 6),
    PCT("Meteor Smasher", 0x176, 1, 6),
    PCT("Shield Master", 0x177, 3, 7),
    PCT("Just Shield Master", 0x178, 4, 6),
    PCT("Shield Grab", 0x179, 6, 6),
    PCT("Item Collector", 0x179, 0, 5),
    PCT("Item Throw To Target", 0x17A, 3, 5),
    PCT("Dragoon Collector", 0x17B, 6, 4),
    PCT("Smash Ball Collector", 0x17B, 2, 4),
    PCT("Hammer Collector", 0x17C, 6, 4),
    PCT("Special Flagger", 0x17C, 2, 4),
    PCT("Item Swinger", 0x17D, 6, 5),
    PCT("Homerun Batter", 0x17D, 1, 4),
    PCT("Club Swinger", 0x17E, 5, 4),
    PCT("Death Swinger", 0x17E, 1, 4),
    PCT("Item Shooter", 0x17F, 5, 5),
    PCT("Carrier Breaker", 0x17F, 0, 5),
    PCT("Charger", 0x180, 3, 5),
    PCT("Taunt", 0x181, 6, 5),
    PCT("Fighter 1", 0x181, 1, 7),
    PCT("Fighter 2", 0x182, 2, 7),
    PCT("Fighter 3", 0x183, 3, 7),
    PCT("Fighter 4", 0x184, 4, 7),
    PCT("Fighter 5", 0x185, 5, 7),
    PCT("Advantageous Fighter", 0x186, 6, 7),
    PCT("Weaken Fighter", 0x187, 7, 7),
    PCT("Revenge", 0x187, 0, 7),
    PCT("Forward Tilt", 0x188, 1, 10),
    PCT("Up Tilt", 0x18A, 7, 10),
    PCT("Down Tilt", 0x18B, 5, 10),
    PCT("Forward Smash", 0x18C, 3, 10),
    PCT("Up Smash", 0x18D, 1, 10),
    PCT("Down Smash", 0x18F, 7, 10),
    PCT("Grounded Neutral Special", 0x190, 5, 10),
    PCT("Grounded Side Special", 0x191, 3, 10),
    PCT("Grounded Up Special", 0x192, 1, 10),
    PCT("Grounded Down Special", 0x194, 7, 10),
    PCT("Forward Air", 0x195, 5, 9),
    PCT("Back Air", 0x196, 4, 9),
    PCT("Up Air", 0x197, 3, 9),
    PCT("Down Air", 0x198, 2, 9),
    PCT("Aerial Neutral Special", 0x199, 1, 9),
    PCT("Aerial Side Special", 0x19A, 0, 9),
    PCT("Aerial Up Special", 0x19C, 7, 9),
    PCT("Aerial Down Special", 0x19D, 6, 9),
    PCT("Forward Air Dodge", 0x19E, 5, 8),
    PCT("Backward Air Dodge", 0x19F, 5, 8),
    PCT("Up Taunt", 0x1A0, 5, 7),
    PCT("Down Taunt", 0x1A1, 6, 7),
};

const size_t smash_field_count = sizeof(smash_fields) / sizeof(smash_fields[0]);

// Percent fields are packed MSB-first: the value's top bit sits at `bit` of
// byte `offset`, then continues downwards and into the following bytes.
static int32_t smash_get_stream(const uint8_t *plain, const smash_field_t *f) {
    uint32_t pos = (uint32_t)f->offset * 8 + (7 - f->bit);
    int32_t value = 0;
    for (uint8_t i = 0; i < f->length; i++, pos++) {
        value = (value << 1) | ((plain[pos >> 3] >> (7 - (pos & 7))) & 1);
    }
    return value;
}

static void smash_set_stream(uint8_t *plain, const smash_field_t *f, int32_t value) {
    uint32_t pos = (uint32_t)f->offset * 8 + (7 - f->bit);
    for (uint8_t i = 0; i < f->length; i++, pos++) {
        uint8_t mask = 1 << (7 - (pos & 7));
        if ((value >> (f->length - 1 - i)) & 1) {
            plain[pos >> 3] |= mask;
        } else {
            plain[pos >> 3] &= ~mask;
        }
    }
}

int32_t smash_data_get(const uint8_t *plain, const smash_field_t *f) {
    switch (f->type) {
    case SMASH_FIELD_UNSIGNED:
        return plain[f->offset] | (plain[f->offset + 1] << 8);
    case SMASH_FIELD_SIGNED:
        return (int16_t)(plain[f->offset] | (plain[f->offset + 1] << 8));
    case SMASH_FIELD_PERCENT:
        return smash_get_stream(plain, f);
    default:
        return (plain[f->offset] >> f->bit) & ((1 << f->length) - 1);
    }
}

void smash_data_set(uint8_t *plain, const smash_field_t *f, int32_t value) {
    if (value < f->min) {
        value = f->min;
    } else if (value > f->max) {
        value = f->max;
    }

    switch (f->type) {
    case SMASH_FIELD_UNSIGNED:
    case SMASH_FIELD_SIGNED:
        plain[f->offset] = value & 0xFF;
        plain[f->offset + 1] = (value >> 8) & 0xFF;
        break;
    case SMASH_FIELD_PERCENT:
        smash_set_stream(plain, f, value);
        break;
    default: {
        uint8_t mask = ((1 << f->length) - 1) << f->bit;
        plain[f->offset] = (plain[f->offset] & ~mask) | ((value << f->bit) & mask);
        break;
    }
    }
}

int32_t smash_data_get_display(const uint8_t *plain, const smash_field_t *f) {
    int32_t value = smash_data_get(plain, f);
    if (f->type == SMASH_FIELD_PERCENT) {
        return (value * 100 + f->max / 2) / f->max;
    }
    return value;
}

void smash_data_set_display(uint8_t *plain, const smash_field_t *f, int32_t value) {
    if (f->type == SMASH_FIELD_PERCENT) {
        if (value < 0) {
            value = 0;
        } else if (value > 100) {
            value = 100;
        }
        value = (value * f->max + 50) / 100;
    }
    smash_data_set(plain, f, value);
}

void smash_data_format(const uint8_t *plain, const smash_field_t *f, char *buf, size_t size) {
    int32_t value = smash_data_get_display(plain, f);
    switch (f->type) {
    case SMASH_FIELD_PERCENT:
        snprintf(buf, size, "%d%%", (int)value);
        break;
    case SMASH_FIELD_BOOL:
        snprintf(buf, size, "%s", value ? "On" : "Off");
        break;
    case SMASH_FIELD_ABILITY:
        snprintf(buf, size, "%s", smash_ability_names[value & 0xFF]);
        break;
    default:
        snprintf(buf, size, "%d", (int)value);
        break;
    }
}
