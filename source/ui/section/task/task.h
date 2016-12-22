#pragma once

#define FILE_NAME_MAX 512
#define FILE_PATH_MAX 512

typedef struct linked_list_s linked_list;
typedef struct list_item_s list_item;

typedef struct meta_info_s {
    char shortDescription[0x100];
    char longDescription[0x200];
    char publisher[0x100];
    u32 region;
    u32 texture;
} meta_info;

typedef struct title_info_s {
    FS_MediaType mediaType;
    u64 titleId;
    char productCode[0x10];
    u16 version;
    u64 installedSize;
    bool twl;
    bool hasMeta;
    meta_info meta;
} title_info;

typedef struct populate_files_data_s {
    linked_list* items;

    FS_Archive archive;
    char path[FILE_PATH_MAX];

    bool recursive;
    bool includeBase;

    bool (*filter)(void* data, const char* name, u32 attributes);
    void* filterData;

    volatile bool finished;
    Result result;
    Handle cancelEvent;
} populate_files_data;

typedef struct ticket_info_s {
    u64 titleId;
} ticket_info;

typedef struct cia_info_s {
    u64 titleId;
    u16 version;
    u64 installedSize;
    bool hasMeta;
    meta_info meta;
} cia_info;

typedef struct file_info_s {
    FS_Archive archive;
    char name[FILE_NAME_MAX];
    char path[FILE_PATH_MAX];
    u32 attributes;

    // Files only
    u64 size;
    bool isCia;
    cia_info ciaInfo;
    bool isTicket;
    ticket_info ticketInfo;
} file_info;

typedef enum data_op_e {
    DATAOP_COPY,
    DATAOP_DELETE
} data_op;

typedef struct data_op_info_s {
    void* data;

    data_op op;

    // Copy
    u32 copyBufferSize;
    bool copyEmpty;

    u32 processed;
    u32 total;

    u64 currProcessed;
    u64 currTotal;

    Result (*isSrcDirectory)(void* data, u32 index, bool* isDirectory);
    Result (*makeDstDirectory)(void* data, u32 index);

    Result (*openSrc)(void* data, u32 index, u32* handle);
    Result (*closeSrc)(void* data, u32 index, bool succeeded, u32 handle);

    Result (*getSrcSize)(void* data, u32 handle, u64* size);
    Result (*readSrc)(void* data, u32 handle, u32* bytesRead, void* buffer, u64 offset, u32 size);

    Result (*openDst)(void* data, u32 index, void* initialReadBlock, u64 size, u32* handle);
    Result (*closeDst)(void* data, u32 index, bool succeeded, u32 handle);

    Result (*writeDst)(void* data, u32 handle, u32* bytesWritten, void* buffer, u64 offset, u32 size);

    Result (*suspendCopy)(void* data, u32 index, u32* srcHandle, u32* dstHandle);
    Result (*restoreCopy)(void* data, u32 index, u32* srcHandle, u32* dstHandle);

    // Delete
    Result (*delete)(void* data, u32 index);

    // Suspend
    Result (*suspend)(void* data, u32 index);
    Result (*restore)(void* data, u32 index);

    // Errors
    bool (*error)(void* data, u32 index, Result res);

    // General
    volatile bool finished;
    Result result;
    Handle cancelEvent;
} data_op_data;

void task_init();
void task_exit();
bool task_is_quit_all();
Handle task_get_pause_event();
Handle task_get_suspend_event();

Result task_data_op(data_op_data* data);

void task_free_file(list_item* item);
void task_clear_files(linked_list* items);
Result task_create_file_item(list_item** out, FS_Archive archive, const char* path, u32 attributes);
Result task_populate_files(populate_files_data* data);
