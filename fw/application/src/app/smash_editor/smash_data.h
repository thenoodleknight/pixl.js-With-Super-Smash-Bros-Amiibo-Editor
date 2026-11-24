#ifndef SMASH_DATA_H
#define SMASH_DATA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SMASH_ABILITY_COUNT 256

typedef enum {
    SMASH_FIELD_UNSIGNED, // little-endian 16-bit
    SMASH_FIELD_SIGNED,   // little-endian 16-bit, two's complement
    SMASH_FIELD_ENUM,     // bit field, MSB-first from `bit` (0 = MSB of the byte)
    SMASH_FIELD_BOOL,     // single bit, addressed like SMASH_FIELD_ENUM
    SMASH_FIELD_ABILITY,  // one byte, index into smash_ability_names
    SMASH_FIELD_PERCENT   // LSB at `bit`, shown as 0-100%
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
    int8_t disp_add;            // added to the stored value when shown/edited (e.g. 1-based skins)
    const char *const *labels;  // optional names for enum values
} smash_field_t;

#define SMASH_NAME_OFFSET 0x38
#define SMASH_NAME_LENGTH 10

extern const smash_field_t smash_fields[];
extern const size_t smash_field_count;
extern const char *const smash_ability_names[SMASH_ABILITY_COUNT];

int32_t smash_data_get(const uint8_t *plain, const smash_field_t *field);
void smash_data_set(uint8_t *plain, const smash_field_t *field, int32_t value);

// Value in the unit used for display/editing (percent fields are 0-100)
int32_t smash_data_get_display(const uint8_t *plain, const smash_field_t *field);
void smash_data_set_display(uint8_t *plain, const smash_field_t *field, int32_t value);

// Percent fields only: value in 1/100000 of a percent (0..10000000)
int32_t smash_data_get_percent_e5(const uint8_t *plain, const smash_field_t *field);
void smash_data_set_percent_e5(uint8_t *plain, const smash_field_t *field, int32_t e5);

void smash_data_format(const uint8_t *plain, const smash_field_t *field, char *buf, size_t size);

// out must hold SMASH_NAME_LENGTH + 1 bytes
void smash_data_get_name(const uint8_t *plain, char *out);
void smash_data_set_name(uint8_t *plain, const char *name);

#endif
