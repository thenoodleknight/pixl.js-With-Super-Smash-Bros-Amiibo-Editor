#include "app_smash_editor.h"
#include "smash_editor_scene.h"

#include "mui_list_view.h"

#define ICON_FILE 0xe1ed
#define ICON_BACK 0xe069
#define ICON_FOLDER 0xe1d6

#define ITEM_BACK 0xFFF0
#define ITEM_PREV 0xFFF1
#define ITEM_NEXT 0xFFF2

// The UI heap is small, so abilities are listed one page at a time
#define PAGE_SIZE 32
#define PAGE_COUNT (SMASH_ABILITY_COUNT / PAGE_SIZE)

static void smash_editor_ability_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item);

static void smash_editor_ability_reload(app_smash_editor_t *app, uint32_t focus_id) {
    mui_list_view_clear_items(app->p_list_view);
    mui_list_view_add_item(app->p_list_view, ICON_BACK, "..", (void *)ITEM_BACK);

    uint32_t focus = 1;
    if (app->ability_page > 0) {
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, "< Previous page", (void *)ITEM_PREV);
        focus++;
    }
    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        uint32_t id = app->ability_page * PAGE_SIZE + i;
        if (id == focus_id) {
            focus = mui_list_view_item_size(app->p_list_view);
        }
        mui_list_view_add_item(app->p_list_view, ICON_FILE, smash_ability_names[id], (void *)(uintptr_t)id);
    }
    if (app->ability_page < PAGE_COUNT - 1) {
        mui_list_view_add_item(app->p_list_view, ICON_FOLDER, "Next page >", (void *)ITEM_NEXT);
    }

    mui_list_view_set_selected_cb(app->p_list_view, smash_editor_ability_on_selected);
    mui_list_view_set_user_data(app->p_list_view, app);
    mui_list_view_set_focus(app->p_list_view, focus);
    mui_view_dispatcher_switch_to_view(app->p_view_dispatcher, SMASH_EDITOR_VIEW_ID_LIST);
}

static void smash_editor_ability_on_selected(mui_list_view_event_t event, mui_list_view_t *p_list_view,
                                             mui_list_item_t *p_item) {
    app_smash_editor_t *app = p_list_view->user_data;
    uint32_t id = (uint32_t)(uintptr_t)p_item->user_data;

    if (event != MUI_LIST_VIEW_EVENT_SELECTED) {
        return;
    }

    // p_item is freed by the reload, so don't touch it afterwards
    if (id == ITEM_PREV) {
        app->ability_page--;
        smash_editor_ability_reload(app, SMASH_ABILITY_COUNT);
        return;
    }
    if (id == ITEM_NEXT) {
        app->ability_page++;
        smash_editor_ability_reload(app, SMASH_ABILITY_COUNT);
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
    uint32_t current = smash_data_get(app->plain, app->p_editing);

    app->ability_page = current / PAGE_SIZE;
    smash_editor_ability_reload(app, current);
}

void smash_editor_scene_ability_on_exit(void *user_data) {
    app_smash_editor_t *app = user_data;
    mui_list_view_set_selected_cb(app->p_list_view, NULL);
    mui_list_view_clear_items(app->p_list_view);
}
