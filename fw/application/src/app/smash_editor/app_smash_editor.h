#ifndef APP_SMASH_EDITOR_H
#define APP_SMASH_EDITOR_H

#include "mini_app_defines.h"
#include "mui_list_view.h"
#include "mui_msg_box.h"
#include "mui_scene_dispatcher.h"
#include "mui_text_input.h"
#include "mui_toast_view.h"
#include "ntag_def.h"
#include "vfs.h"

#include "mlib_common.h"

#include "smash_data.h"

#define SMASH_PLAIN_SIZE 540

typedef struct {
    mui_list_view_t *p_list_view;
    mui_text_input_t *p_text_input;
    mui_msg_box_t *p_msg_box;
    mui_toast_view_t *p_toast_view;
    mui_view_dispatcher_t *p_view_dispatcher;
    mui_view_dispatcher_t *p_view_dispatcher_toast;
    mui_scene_dispatcher_t *p_scene_dispatcher;

    /** file browser */
    vfs_drive_t current_drive;
    string_t current_folder;
    string_t current_file;
    uint32_t current_focus_index;

    /** loaded amiibo */
    ntag_t *p_ntag;
    uint8_t plain[SMASH_PLAIN_SIZE];
    bool dirty;

    /** editing state */
    uint16_t group_focus[3];
    const smash_field_t *p_editing;
} app_smash_editor_t;

typedef enum {
    SMASH_EDITOR_VIEW_ID_LIST,
    SMASH_EDITOR_VIEW_ID_INPUT,
    SMASH_EDITOR_VIEW_ID_MSG_BOX,
    SMASH_EDITOR_VIEW_ID_TOAST
} smash_editor_view_id_t;

extern mini_app_t app_smash_editor_info;

#endif
