#include "app_smash_editor.h"
#include "smash_editor_scene.h"

#include "amiibo_helper.h"
#include "cwalk.h"
#include "cwalk2.h"
#include "mini_app_launcher.h"
#include "mini_app_registry.h"
#include "mui_list_view.h"
#include "vfs.h"
#include "vfs_meta.h"

#include "i18n/language.h"

#define ICON_FOLDER 0xe1d6
#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_ERROR 0xe1bb
#define ICON_HOME 0xe1f0

#define FOLDER_LIST_PARENT 0xFFFF

static int smash_editor_file_browser_item_cmp(const mui_list_item_t *p_item_a, const mui_list_item_t *p_item_b) {
    if (p_item_a->icon == ICON_HOME) {
        return -1;
    }
    if (p_item_b->icon == ICON_HOME) {
        return 1;
    }
    if (p_item_a->icon != p_item_b->icon) {
        return p_item_a->icon - p_item_b->icon;
    }
    return string_cmp(p_item_a->text, p_item_b->text);
}

static void smash_editor_file_browser_reload(app_smash_editor_t *app) {
    vfs_dir_t dir;
    vfs_obj_t obj;

    mui_list_view_clear_items(app->p_list_view);
    if (string_cmp_str(app->current_folder, "/") == 0) {
        mui_list_view_add_item(app->p_list_view, ICON_HOME, _T(MAIN_MENU), (void *)FOLDER_LIST_PARENT);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)FOLDER_LIST_PARENT);
    }

    vfs_driver_t *p_vfs_driver = vfs_get_driver(app->current_drive);
    if (p_vfs_driver->open_dir(string_get_cstr(app->current_folder), &dir) == VFS_OK) {
        while (p_vfs_driver->read_dir(&dir, &obj) == VFS_OK) {
            vfs_meta_t meta;
            memset(&meta, 0, sizeof(vfs_meta_t));
            vfs_meta_decode(obj.meta, sizeof(obj.meta), &meta);
            if (meta.has_flags && (meta.flags & VFS_OBJ_FLAG_HIDDEN)) {
                continue;
            }
            uint16_t icon = obj.type == VFS_TYPE_DIR ? ICON_FOLDER : ICON_FILE;
            mui_list_view_add_item(app->p_list_view, icon, obj.name, (void *)-1);
        }
        p_vfs_driver->close_dir(&dir);
    } else {
        mui_list_view_add_item(app->p_list_view, ICON_ERROR, getLangString(_L_OPEN_FOLDER_FAILED), (void *)-1);
    }

    mui_list_view_sort(app->p_list_view, smash_editor_file_browser_item_cmp);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
}

// Reads the selected file and decrypts it into app->plain
static bool smash_editor_file_browser_load(app_smash_editor_t *app) {
    char path[VFS_MAX_PATH_LEN];
    vfs_obj_t obj;

    if (!amiibo_helper_is_key_loaded()) {
        mui_toast_view_show(app->p_toast_view, "key_retail.bin not found");
        return false;
    }

    cwalk_append_segment(path, string_get_cstr(app->current_folder), string_get_cstr(app->current_file));

    vfs_driver_t *p_driver = vfs_get_driver(app->current_drive);
    if (p_driver->stat_file(path, &obj) != VFS_OK) {
        mui_toast_view_show(app->p_toast_view, string_get_cstr(app->current_file));
        return false;
    }
    if (obj.size != NTAG_DATA_SIZE && obj.size != NTAG_I2C_2K_DATA_SIZE) {
        mui_toast_view_show(app->p_toast_view, _T(NOT_AMIIBO_FILE));
        return false;
    }

    ntag_t *ntag = app->p_ntag;
    memset(ntag, 0, sizeof(ntag_t));
    ntag->type = _ntag_type(obj.size);
    if (p_driver->read_file_data(path, ntag->data, obj.size) != (int32_t)obj.size) {
        mui_toast_view_show(app->p_toast_view, _T(READ_FILE_FAILED));
        return false;
    }

    if (amiibo_helper_unpack(ntag, app->plain) != NRF_SUCCESS) {
        mui_toast_view_show(app->p_toast_view, "Invalid amiibo or wrong key");
        return false;
    }

    app->dirty = false;
    memset(app->group_focus, 0, sizeof(app->group_focus));
    return true;
}

static void smash_editor_file_browser_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                                  mui_list_item_t *p_item) {
    app_smash_editor_t *app = p_list_view->user_data;
    uint32_t idx = (uint32_t)p_item->user_data;

    if (event != MUI_LIST_VIEW_EVENT_SELECTED) {
        return;
    }

    string_set(app->current_file, p_item->text);

    if (idx == FOLDER_LIST_PARENT) {
        if (string_cmp_str(app->current_folder, "/") == 0) {
            mini_app_launcher_kill(mini_app_launcher(), MINI_APP_ID_SMASH_EDITOR);
        } else {
            struct cwk_segment segment;
            const char *folder_cstr = string_get_cstr(app->current_folder);
            cwk_path_get_last_segment(folder_cstr, &segment);
            string_left(app->current_folder, segment.begin - folder_cstr);
            if (string_size(app->current_folder) == 0) {
                string_cat_str(app->current_folder, "/");
            }
            smash_editor_file_browser_reload(app);
        }
    } else if (p_item->icon == ICON_FOLDER) {
        if (!string_end_with_str_p(app->current_folder, "/")) {
            string_cat_str(app->current_folder, "/");
        }
        string_cat(app->current_folder, p_item->text);
        smash_editor_file_browser_reload(app);
    } else if (p_item->icon == ICON_FILE) {
        app->current_focus_index = mui_list_view_get_focus(p_list_view);
        if (smash_editor_file_browser_load(app)) {
            mui_scene_dispatcher_next_scene(app->p_scene_dispatcher, SMASH_EDITOR_SCENE_MAIN);
        }
    }
}

void smash_editor_scene_file_browser_on_enter(void *user_data) {
    app_smash_editor_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, smash_editor_file_browser_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);

    smash_editor_file_browser_reload(app);
    mui_list_view_set_focus(app->p_list_view, app->current_focus_index);
}

void smash_editor_scene_file_browser_on_exit(void *user_data) {
    app_smash_editor_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_set_user_data(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
