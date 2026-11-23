#ifndef SMASH_EDITOR_SCENE_H
#define SMASH_EDITOR_SCENE_H

#include "mui_scene_dispatcher.h"

#define ADD_SCENE(prefix, name, id) SMASH_EDITOR_SCENE_##id,
typedef enum {
#include "smash_editor_scene_config.h"
    SMASH_EDITOR_SCENE_MAX,
} smash_editor_scene_id_t;
#undef ADD_SCENE

extern const mui_scene_t smash_editor_scene_defines[];

#endif
