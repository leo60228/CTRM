#include <malloc.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <cstring>

#include <3ds.h>

extern "C" {
    #include "list.h"
    #include "mainmenu.h"
    #include "ui.h"
    #include "section/section.h"
    #include "../core/linkedlist.h"
    #include "../core/screen.h"
}

static list_item pluginfs_entry = {"SD", COLOR_TEXT, (void*)(int)0};
static std::vector<u64> pluginids;

static void mainmenu_launch_title(u64 tid) {
    Result res = 0;

    if(R_SUCCEEDED(res = APT_PrepareToDoApplicationJump(0, tid, (u8)3))) {
        u8 param[0x300];
        u8 hmac[0x20];

        res = APT_DoApplicationJump(param, sizeof(param), hmac);
    }
}

static void mainmenu_run_plugin(u64 pluginid) {
    if (pluginid == 0) {
        files_open_pluginfs();
    } else {
        mainmenu_launch_title(pluginid);
    }
}

static void mainmenu_draw_top(ui_view* view, void* data, float x1, float y1, float x2, float y2, list_item* selected) {
    u32 logoWidth;
    u32 logoHeight;
    screen_get_texture_size(&logoWidth, &logoHeight, TEXTURE_LOGO);

    float logoX = x1 + (x2 - x1 - logoWidth) / 2;
    float logoY = y1 + (y2 - y1 - logoHeight) / 2;
    screen_draw_texture(TEXTURE_LOGO, logoX, logoY, logoWidth, logoHeight);
}

static std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

static std::string readFile2(const std::string &fileName) {
    std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);

    return std::string(&bytes[0], fileSize);
}

static void mainmenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched) {
    if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A)) {
        mainmenu_run_plugin((u64)(selected->data));
        return;
    }

    if(linked_list_size(items) == 0) {
        pluginids.push_back(0);
        linked_list_add(items, &pluginfs_entry);
        if(access("ctrm/plugins.txt", F_OK) != -1) {
            std::string filename = "ctrm/plugins.txt";
            
            std::string pluginsfile = readFile2(filename);

            int i = 1;
            
            char* cfile = const_cast<char*>(pluginsfile.c_str());

            char* line = strtok(cfile, "\n");
            
            std::ofstream log("ctrmlog.txt");

            while (line != NULL) {
                std::string stdline = line;
                
                log << stdline << "\n";

                std::vector<std::string> plugininfo = split(stdline, ' ');
                
                log << plugininfo[0] << "\n";
                log << plugininfo[1] << "\n";

                list_item plugin_entry;
                strcpy(plugin_entry.name, plugininfo[1].c_str());
                plugin_entry.color = COLOR_TEXT;
                plugin_entry.data = (void*)i;

                linked_list_add(items, &plugin_entry);

                pluginids.push_back(strtoull(plugininfo[0].c_str(), NULL, 10));
                i++;
                line = strtok(NULL, "\n");
            }
        }
    }
}

void mainmenu_open() {
    list_display("Main Menu", "A: Select, START: Exit", NULL, mainmenu_update, mainmenu_draw_top);
}
