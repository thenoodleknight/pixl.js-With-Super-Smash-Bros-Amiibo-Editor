#include "app_smash_editor.h"
#include "mini_app_registry.h"

#include "mui_include.h"

#include "smash_editor_scene.h"

#include "amiibo_helper.h"
#include "i18n/language.h"
#include "m-string.h"

static void app_smash_editor_on_run(mini_app_inst_t *p_app_inst);
static void app_smash_editor_on_kill(mini_app_inst_t *p_app_inst);
static void app_smash_editor_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event);

void app_smash_editor_on_run(mini_app_inst_t *p_app_inst) {
    app_smash_editor_t *p_app_handle = mui_mem_malloc(sizeof(app_smash_editor_t));
    memset(p_app_handle, 0, sizeof(app_smash_editor_t));
    p_app_inst->p_handle = p_app_handle;

    p_app_handle->p_ntag = mui_mem_malloc(sizeof(ntag_t));
    memset(p_app_handle->p_ntag, 0, sizeof(ntag_t));

    p_app_handle->p_view_dispatcher = mui_view_dispatcher_create();
    p_app_handle->p_list_view = mui_list_view_create();
    mui_list_view_set_user_data(p_app_handle->p_list_view, p_app_handle);
    p_app_handle->p_text_input = mui_text_input_create();
    mui_text_input_set_user_data(p_app_handle->p_text_input, p_app_handle);
    p_app_handle->p_msg_box = mui_msg_box_create();
    mui_msg_box_set_user_data(p_app_handle->p_msg_box, p_app_handle);
    p_app_handle->p_toast_view = mui_toast_view_create();
    mui_toast_view_set_user_data(p_app_handle->p_toast_view, p_app_handle);

    p_app_handle->p_scene_dispatcher = mui_scene_dispatcher_create();

    string_init(p_app_handle->current_file);
    string_init(p_app_handle->current_folder);

    mui_scene_dispatcher_set_user_data(p_app_handle->p_scene_dispatcher, p_app_handle);
    mui_scene_dispatcher_set_scene_defines(p_app_handle->p_scene_dispatcher, smash_editor_scene_defines,
                                           SMASH_EDITOR_SCENE_MAX);

    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST,
                                 mui_list_view_get_view(p_app_handle->p_list_view));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_INPUT,
                                 mui_text_input_get_view(p_app_handle->p_text_input));
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_MSG_BOX,
                                 mui_msg_box_get_view(p_app_handle->p_msg_box));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);

    p_app_handle->p_view_dispatcher_toast = mui_view_dispatcher_create();
    mui_view_dispatcher_add_view(p_app_handle->p_view_dispatcher_toast, SMASH_EDITOR_VIEW_ID_TOAST,
                                 mui_toast_view_get_view(p_app_handle->p_toast_view));
    mui_view_dispatcher_attach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_switch_to_view(p_app_handle->p_view_dispatcher_toast, SMASH_EDITOR_VIEW_ID_TOAST);

    p_app_handle->current_drive = vfs_get_default_drive();
    string_set_str(p_app_handle->current_folder, "/");

    vfs_driver_t *p_driver = vfs_get_driver(p_app_handle->current_drive);
    if (!p_driver->mounted()) {
        p_driver->mount();
    }
    amiibo_helper_try_load_amiibo_keys_from_vfs();

    mui_scene_dispatcher_next_scene(p_app_handle->p_scene_dispatcher, SMASH_EDITOR_SCENE_FILE_BROWSER);
}

void app_smash_editor_on_kill(mini_app_inst_t *p_app_inst) {
    app_smash_editor_t *p_app_handle = p_app_inst->p_handle;

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher, MUI_LAYER_FULLSCREEN);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher);
    mui_list_view_free(p_app_handle->p_list_view);
    mui_text_input_free(p_app_handle->p_text_input);
    mui_msg_box_free(p_app_handle->p_msg_box);
    mui_scene_dispatcher_free(p_app_handle->p_scene_dispatcher);

    mui_view_dispatcher_detach(p_app_handle->p_view_dispatcher_toast, MUI_LAYER_TOAST);
    mui_view_dispatcher_free(p_app_handle->p_view_dispatcher_toast);
    mui_toast_view_free(p_app_handle->p_toast_view);

    string_clear(p_app_handle->current_file);
    string_clear(p_app_handle->current_folder);

    mui_mem_free(p_app_handle->p_ntag);
    mui_mem_free(p_app_handle);

    p_app_inst->p_handle = NULL;
}

void app_smash_editor_on_event(mini_app_inst_t *p_app_inst, mini_app_event_t *p_event) {}

mini_app_t app_smash_editor_info = {.id = MINI_APP_ID_SMASH_EDITOR,
                                    .name = "Smash Editor",
                                    .name_i18n_key = _L_APP_SMASH_EDITOR,
                                    .icon = 0xe082,
                                    .sys = false,
                                    .deamon = false,
                                    .hibernate_enabled = false,
                                    .icon_32x32 = &app_game_32x32,
                                    .run_cb = app_smash_editor_on_run,
                                    .kill_cb = app_smash_editor_on_kill,
                                    .on_event_cb = app_smash_editor_on_event};
