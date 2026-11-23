#include "app_smash_editor.h"
#include "smash_editor_scene.h"

#include "amiibo_helper.h"
#include "cwalk2.h"
#include "i18n/language.h"
#include "mui_list_view.h"
#include "vfs.h"

#include <stdio.h>
#include <stdlib.h>

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069

#define ITEM_BACK 0xFFF0
#define ITEM_PERSONALITY 0xFFF1
#define ITEM_MII 0xFFF2
#define ITEM_SAVE 0xFFF3

// list item user_data for a field is (index into smash_fields) + 1
#define FIELD_TO_DATA(i) ((void *)(uintptr_t)((i) + 1))
#define DATA_TO_FIELD(d) ((uint32_t)(uintptr_t)(d)-1)

static void smash_editor_fields_reload(app_smash_editor_t *app, smash_group_t group, uint16_t focus);

static smash_group_t smash_editor_current_group(app_smash_editor_t *app) {
    switch (mui_scene_dispatcher_current_scene(app->p_scene_dispatcher)) {
    case SMASH_EDITOR_SCENE_PERSONALITY:
        return SMASH_GROUP_PERSONALITY;
    case SMASH_EDITOR_SCENE_MII:
        return SMASH_GROUP_MII;
    default:
        return SMASH_GROUP_MAIN;
    }
}

static void smash_editor_save(app_smash_editor_t *app) {
    char path[VFS_MAX_PATH_LEN];

    if (amiibo_helper_pack(app->p_ntag, app->plain) != NRF_SUCCESS) {
        mui_toast_view_show(app->p_toast_view, "Failed");
        return;
    }

    cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));
    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    if (p_driver->write_file_data(path, app->p_ntag->data, _ntag_data_size(app->p_ntag)) < 0) {
        mui_toast_view_show(app->p_toast_view, "Failed");
        return;
    }

    app->dirty = false;
    mui_toast_view_show(app->p_toast_view, "Saved");
}

static void smash_editor_unsaved_msg_box_cb(mui_msg_box_event_t event, mui_msg_box_t *p_msg_box) {
    app_smash_editor_t *app = p_msg_box->user_data;

    switch (event) {
    case MUI_MSG_BOX_EVENT_SELECT_LEFT:
        smash_editor_save(app);
        if (app->dirty) {
            mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
            return;
        }
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    case MUI_MSG_BOX_EVENT_SELECT_CENTER:
        app->dirty = false;
        mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        break;
    default:
        mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
        break;
    }
}

static void smash_editor_show_unsaved_msg_box(app_smash_editor_t *app) {
    mui_msg_box_set_header(app->p_msg_box, "Unsaved changes");
    mui_msg_box_set_message(app->p_msg_box, "Save before leaving?");
    mui_msg_box_set_btn_text(app->p_msg_box, "Save", "Discard", _T(CANCEL));
    mui_msg_box_set_btn_focus(app->p_msg_box, 0);
    mui_msg_box_set_event_cb(app->p_msg_box, smash_editor_unsaved_msg_box_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_MSG_BOX);
}

static bool smash_editor_parse_number(const char *text, int32_t *p_value) {
    bool negative = false;
    int32_t value = 0;

    // the on-screen keyboard has no minus sign, "_" stands in for it
    if (*text == '_' || *text == '-') {
        negative = true;
        text++;
    }
    if (*text == '\0') {
        return false;
    }
    for (; *text; text++) {
        if (*text < '0' || *text > '9') {
            return false;
        }
        value = value * 10 + (*text - '0');
        if (value > 100000) {
            return false;
        }
    }
    *p_value = negative ? -value : value;
    return true;
}

static void smash_editor_text_input_cb(mui_text_input_event_t event, mui_text_input_t *p_text_input) {
    app_smash_editor_t *app = p_text_input->user_data;
    const smash_field_t *field = app->p_editing;
    int32_t value;

    if (event == MUI_TEXT_INPUT_EVENT_CONFIRMED) {
        int32_t min = field->min;
        int32_t max = field->type == SMASH_FIELD_PERCENT ? 100 : field->max;

        if (!smash_editor_parse_number(mui_text_input_get_input_text(p_text_input), &value) || value < min ||
            value > max) {
            mui_toast_view_show(app->p_toast_view, "Invalid value");
            return;
        }
        smash_data_set_display(app->plain, field, value);
        app->dirty = true;
    }

    smash_group_t group = smash_editor_current_group(app);
    smash_editor_fields_reload(app, group, app->group_focus[group]);
}

static void smash_editor_start_number_input(app_smash_editor_t *app, const smash_field_t *field) {
    char header[40];
    char text[16];
    int32_t max = field->type == SMASH_FIELD_PERCENT ? 100 : field->max;

    snprintf(header, sizeof(header), "%s %d..%d", field->name, (int)field->min, (int)max);
    snprintf(text, sizeof(text), "%d", (int)smash_data_get_display(app->plain, field));

    app->p_editing = field;
    mui_text_input_set_header(app->p_text_input, header);
    mui_text_input_set_input_text(app->p_text_input, text);
    mui_text_input_set_event_cb(app->p_text_input, smash_editor_text_input_cb);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_INPUT);
}

static void smash_editor_fields_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                            mui_list_item_t *p_item) {
    app_smash_editor_t *app = p_list_view->user_data;
    uint32_t code = (uint32_t)(uintptr_t)p_item->user_data;
    smash_group_t group = smash_editor_current_group(app);

    if (event != MUI_LIST_VIEW_EVENT_SELECTED) {
        return;
    }
    app->group_focus[group] = mui_list_view_get_focus(p_list_view);

    switch (code) {
    case ITEM_BACK:
        if (group == SMASH_GROUP_MAIN && app->dirty) {
            smash_editor_show_unsaved_msg_box(app);
        } else {
            mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
        }
        return;
    case ITEM_PERSONALITY:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SMASH_EDITOR_SCENE_PERSONALITY);
        return;
    case ITEM_MII:
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SMASH_EDITOR_SCENE_MII);
        return;
    case ITEM_SAVE:
        smash_editor_save(app);
        return;
    default:
        break;
    }

    const smash_field_t *field = &smash_fields[DATA_TO_FIELD(p_item->user_data)];
    switch (field->type) {
    case SMASH_FIELD_ABILITY:
        app->p_editing = field;
        mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SMASH_EDITOR_SCENE_ABILITY);
        return;
    case SMASH_FIELD_BOOL:
    case SMASH_FIELD_ENUM:
        if (field->max > 15) {
            smash_editor_start_number_input(app, field);
            return;
        }
        {
            int32_t value = smash_data_get(app->plain, field) + 1;
            smash_data_set(app->plain, field, value > field->max ? field->min : value);
            app->dirty = true;
            smash_editor_fields_reload(app, group, app->group_focus[group]);
        }
        return;
    default:
        smash_editor_start_number_input(app, field);
        return;
    }
}

static void smash_editor_fields_reload(app_smash_editor_t *app, smash_group_t group, uint16_t focus) {
    char value[48];

    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)ITEM_BACK);

    for (size_t i = 0; i < smash_field_count; i++) {
        const smash_field_t *field = &smash_fields[i];
        if (field->group != group) {
            continue;
        }
        smash_data_format(app->plain, field, value, sizeof(value));
        mui_list_view_add_item_ext(app->p_list_view, ICON_FILE, field->name, value, FIELD_TO_DATA(i));
    }

    if (group == SMASH_GROUP_MAIN) {
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, "Personality", (void *)ITEM_PERSONALITY);
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, "Mii Fighter", (void *)ITEM_MII);
        mui_list_view_add_item(app->p_list_view, ICON_FILE, app->dirty ? "Save *" : "Save", (void *)ITEM_SAVE);
    }

    mui_list_view_set_selected_cb(app->p_list_view, smash_editor_fields_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_list_view_set_focus(app->p_list_view, focus);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
}

static void smash_editor_fields_on_exit(app_smash_editor_t *app, smash_group_t group) {
    app->group_focus[group] = mui_list_view_get_focus(app->p_list_view);
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}

void smash_editor_scene_main_on_enter(void *user_data) {
    app_smash_editor_t *app = user_data;
    smash_editor_fields_reload(app, SMASH_GROUP_MAIN, app->group_focus[SMASH_GROUP_MAIN]);
}

void smash_editor_scene_main_on_exit(void *user_data) { smash_editor_fields_on_exit(user_data, SMASH_GROUP_MAIN); }

void smash_editor_scene_personality_on_enter(void *user_data) {
    app_smash_editor_t *app = user_data;
    smash_editor_fields_reload(app, SMASH_GROUP_PERSONALITY, app->group_focus[SMASH_GROUP_PERSONALITY]);
}

void smash_editor_scene_personality_on_exit(void *user_data) {
    smash_editor_fields_on_exit(user_data, SMASH_GROUP_PERSONALITY);
}

void smash_editor_scene_mii_on_enter(void *user_data) {
    app_smash_editor_t *app = user_data;
    smash_editor_fields_reload(app, SMASH_GROUP_MII, app->group_focus[SMASH_GROUP_MII]);
}

void smash_editor_scene_mii_on_exit(void *user_data) { smash_editor_fields_on_exit(user_data, SMASH_GROUP_MII); }
