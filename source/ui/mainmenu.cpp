#include <malloc.h>
#include <stdio.h>

#include <3ds.h>

extern "C" {
    #include "list.h"
    #include "mainmenu.h"
    #include "ui.h"
    #include "section/section.h"
    #include "../core/linkedlist.h"
    #include "../core/screen.h"
}

struct run_plugin {
    run_plugin(u64 tid) : tid(tid) {}
    void operator()() {
        if (tid == 0) {
            files_open_pluginfs();
            return;
        }
    }

private:
    u64 tid;
};

typedef struct plugin_list_item_s {
    char name[LIST_ITEM_NAME_MAX];
    u32 color;
    run_plugin runner;
} plugin_list_item;

static list_item pluginfs_entry = {"SD", COLOR_TEXT, (void*)(int)0};
static plugin_list_item fsplugins[64];

/*extern "C" {
    void mainmenu_update(ui_view*, void*, linked_list*, list_item*, bool);

    void mainmenu_open();

    void mainmenu_draw_top(ui_view*, void*, float, float, float, float, list_item*);
}*/
static void mainmenu_draw_top(ui_view* view, void* data, float x1, float y1, float x2, float y2, list_item* selected) {
    u32 logoWidth;
    u32 logoHeight;
    screen_get_texture_size(&logoWidth, &logoHeight, TEXTURE_LOGO);

    float logoX = x1 + (x2 - x1 - logoWidth) / 2;
    float logoY = y1 + (y2 - y1 - logoHeight) / 2;
    screen_draw_texture(TEXTURE_LOGO, logoX, logoY, logoWidth, logoHeight);
}

static void mainmenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched) {
    if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A)) {
        fsplugins[(int)(selected->data)].runner();
        return;
    }

    if(linked_list_size(items) == 0) {
        plugin_list_item pluginfs = {"SD", COLOR_TEXT, run_plugin((u64)0)};
        fsplugins[0] = pluginfs;
        linked_list_add(items, &pluginfs_entry);
    }
}

void mainmenu_open() {
    list_display("Main Menu", "A: Select, START: Exit", NULL, mainmenu_update, mainmenu_draw_top);
}
