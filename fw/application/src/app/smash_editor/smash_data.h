#ifndef SMASH_DATA_H
#define SMASH_DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SMASH_ABILITY_COUNT 256

typedef enum {
    SMASH_FIELD_UNSIGNED, // little-endian 16-bit
    SMASH_FIELD_SIGNED,   // little-endian 16-bit, two's complement
    SMASH_FIELD_ENUM,     // little-endian bit field inside a single byte
    SMASH_FIELD_BOOL,     // single bit
    SMASH_FIELD_ABILITY,  // one byte, index into smash_ability_names
    SMASH_FIELD_PERCENT   // MSB-first bit stream, shown as 0-100%
} smash_field_type_t;

typedef enum { SMASH_GROUP_MAIN, SMASH_GROUP_MII, SMASH_GROUP_PERSONALITY } smash_group_t;

typedef struct {
    const char *name;
    uint16_t offset; // offset in the decrypted (internal) amiibo layout
    uint8_t bit;
    uint8_t length; // in bits, 16 for 16-bit integers
    uint8_t type;
    uint8_t group;
    int32_t min;
    int32_t max;
} smash_field_t;

extern const smash_field_t smash_fields[];
extern const size_t smash_field_count;
extern const char *const smash_ability_names[SMASH_ABILITY_COUNT];

int32_t smash_data_get(const uint8_t *plain, const smash_field_t *field);
void smash_data_set(uint8_t *plain, const smash_field_t *field, int32_t value);

// Value in the unit used for display/editing (percent fields are 0-100)
int32_t smash_data_get_display(const uint8_t *plain, const smash_field_t *field);
void smash_data_set_display(uint8_t *plain, const smash_field_t *field, int32_t value);

void smash_data_format(const uint8_t *plain, const smash_field_t *field, char *buf, size_t size);

#endif
