#include <windows.h>
#include <stdint.h>

typedef struct File_Watcher {
	HANDLE dir_watcher;
	OVERLAPPED overlapped;
	uint8_t buffer[1024];

	int init_watcher(const char*);
	void de_init_watcher();
	void add_file_to_watch(char **);
	const char *get_changed_files();
	
} File_Watcher;

int File_Watcher::init_watcher(const char *file_path) {
	dir_watcher = CreateFile((LPSTR)file_path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_BACKUP_SEMANTICS, nullptr);
	if (dir_watcher == INVALID_HANDLE_VALUE) {
		return 0;
	}
	overlapped.hEvent = CreateEvent(nullptr, true, false, nullptr);
	if(!ReadDirectoryChangesW(dir_watcher, buffer, sizeof(buffer), false, FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME, nullptr, &overlapped, nullptr)) {
		CloseHandle(dir_watcher);
		CloseHandle(overlapped.hEvent);
		return 0;
	}
	return 1;
}

void File_Watcher::de_init_watcher() {
	CancelIo(dir_watcher);
	CloseHandle(dir_watcher);
	CloseHandle(overlapped.hEvent);
}

void File_Watcher::add_file_to_watch(char **file_path) {
	
}

const char *File_Watcher::get_changed_files() {
	return "nothing";	
}
