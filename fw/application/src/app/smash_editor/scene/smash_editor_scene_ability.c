#include "app_smash_editor.h"
#include "smash_editor_scene.h"

#include "mui_list_view.h"

#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069

#define ITEM_BACK 0xFFF0

static void smash_editor_ability_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_smash_editor_t *app = p_list_view->user_data;
    uint32_t id = (uint32_t)(uintptr_t)p_item->user_data;

    if (event != MUI_LIST_VIEW_EVENT_SELECTED) {
        return;
    }

    if (id != ITEM_BACK) {
        smash_data_set(app->plain, app->p_editing, id);
        app->dirty = true;
    }
    mui_scene_dispatcher_previous_scene(app->p_scene_dispatcher);
}

void smash_editor_scene_ability_on_enter(void *user_data) {
    app_smash_editor_t *app = user_data;

    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)ITEM_BACK);
    for (uint32_t id = 0; id < SMASH_ABILITY_COUNT; id++) {
        mui_list_view_add_item(app->p_list_view, ICON_FILE, smash_ability_names[id], (void *)(uintptr_t)id);
    }

    mui_list_view_set_selected_cb(app->p_list_view, smash_editor_ability_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    // +1 for the ".." entry
    mui_list_view_set_focus(app->p_list_view, smash_data_get(app->plain, app->p_editing) + 1);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
}

void smash_editor_scene_ability_on_exit(void *user_data) {
    app_smash_editor_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
