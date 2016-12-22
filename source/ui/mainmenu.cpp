#include <malloc.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

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

static void mainmenu_launch_title(uint64_t tid) {
    Result res = 0;

    if(R_SUCCEEDED(res = APT_PrepareToDoApplicationJump(0, tid, (u8)1))) {
        u8 param[0x300] = {0};
        u8 hmac[0x20] = {0};

        res = APT_DoApplicationJump(param, sizeof(param), hmac);
    }
}

static void mainmenu_run_plugin(uint64_t pluginid) {
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

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrimmed(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrimmed(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trimmed(std::string s) {
    trim(s);
    return s;
}

static void mainmenu_update(ui_view* view, void* data, linked_list* items, list_item* selected, bool selectedTouched) {
    if(selected != NULL && (selectedTouched || hidKeysDown() & KEY_A)) {
        mainmenu_run_plugin(pluginids[(u64)(selected->data)]);
        return;
    }

    if(linked_list_size(items) == 0) {
        pluginids.push_back(0);

        if(access("ctrm/fsplugins.txt", F_OK) != -1) {
            std::string filename = "ctrm/fsplugins.txt";
            
            std::string pluginsfile = readFile2(filename);

            int i = 1;
            
            const char* cfile = pluginsfile.c_str();
            
            char varcfile[512];

            strcpy(varcfile, cfile);
            
            char* line = strtok(varcfile, "\n");
            
            std::ofstream log("logfile.txt", std::ios_base::app | std::ios_base::out);

            log << pluginids[0] << "\n";

            while (line != NULL) {
                std::string stdline = line;
                
                std::vector<std::string> plugininfo = split(stdline, ' ');
                
                char cname[512];
                strcpy(cname, rtrimmed(plugininfo[1]).c_str());

                list_item* plugin_entry = new list_item;
                strcpy(plugin_entry->name, cname);
                plugin_entry->color = COLOR_TEXT;
                plugin_entry->data = (void*)i;

                linked_list_add(items, plugin_entry);

                char titleid[24];
                strcpy(titleid, plugininfo[0].c_str());
                
                uint64_t thistitle = 0x000400000F805600;
                uint64_t currenttitle = (uint64_t)strtoull(titleid, NULL, 16);
                uint64_t pluginid = (thistitle == currenttitle) ? 0 : currenttitle;
                
                log << pluginid << "\n";

                pluginids.push_back(pluginid);

                i++;
                line = strtok(NULL, "\n");
            }
        } else {
            linked_list_add(items, &pluginfs_entry);
        }
    }
}

void mainmenu_open() {
    list_display("Main Menu", "A: Select, START: Exit", NULL, mainmenu_update, mainmenu_draw_top);
}
