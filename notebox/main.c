#include "notebox.h"


NoteBox* notebox = NULL;
int shm_fd = -1;
int is_creator = 0;
char current_user[32];

void get_timestamp(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M", tm_info);
}

void cleanup_and_exit(int signum) {
    pthread_mutex_lock(&notebox->mutex);
    notebox->user_count--;
    int last_user = (notebox->user_count == 0);
    pthread_mutex_unlock(&notebox->mutex);

    if (last_user) {
        shm_unlink(SHM_NAME);
    }
    munmap(notebox, sizeof(NoteBox));
    close(shm_fd);
    printf("\nExiting NoteBox. Bye!\n");
    exit(0);
}

void init_shared_memory() {
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);
    if (shm_fd >= 0) {
        is_creator = 1;
        ftruncate(shm_fd, sizeof(NoteBox));
    } else if (errno == EEXIST) {
        shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
        is_creator = 0;
    } else {
        perror("shm_open");
        exit(1);
    }

    notebox = mmap(NULL, sizeof(NoteBox), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (notebox == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    if (is_creator) {
        memset(notebox, 0, sizeof(NoteBox));
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&notebox->mutex, &attr);
        pthread_mutexattr_destroy(&attr);
        notebox->user_count = 0;
    }

    pthread_mutex_lock(&notebox->mutex);
    notebox->user_count++;
    pthread_mutex_unlock(&notebox->mutex);
}

void view_notes() {
    pthread_mutex_lock(&notebox->mutex);
    for (int i = 0; i < MAX_NOTES; i++) {
        if (notebox->notes[i].active) {
            printf("[%d] by %s at %s\n%s\n\n", i,
                notebox->notes[i].author,
                notebox->notes[i].timestamp,
                notebox->notes[i].text);
        }
    }
    pthread_mutex_unlock(&notebox->mutex);
}

void add_note() {
    char text[MAX_TEXT];
    printf("Enter your note (max 200 characters):\n");
    fgets(text, MAX_TEXT, stdin);
    text[strcspn(text, "\n")] = '\0'; // remove newline

    pthread_mutex_lock(&notebox->mutex);
    int added = 0;
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notebox->notes[i].active) {
            strncpy(notebox->notes[i].author, current_user, 31);
            get_timestamp(notebox->notes[i].timestamp, 32);
            strncpy(notebox->notes[i].text, text, MAX_TEXT - 1);
            notebox->notes[i].active = 1;
            added = 1;
            break;
        }
    }
    pthread_mutex_unlock(&notebox->mutex);

    if (!added) {
        printf("NoteBox is full. Cannot add new note.\n");
    }
}

void delete_note() {
    int index;
    printf("Enter index of note to delete: ");
    scanf("%d", &index);
    while (getchar() != '\n'); // clear input buffer

    pthread_mutex_lock(&notebox->mutex);
    if (index < 0 || index >= MAX_NOTES || !notebox->notes[index].active) {
        printf("Invalid note index.\n");
    } else if (strcmp(notebox->notes[index].author, current_user) != 0) {
        printf("You can only delete your own notes.\n");
    } else {
        notebox->notes[index].active = 0;
        printf("Note deleted successfully.\n");
    }
    pthread_mutex_unlock(&notebox->mutex);
}

int main() {
    signal(SIGINT, cleanup_and_exit);
    init_shared_memory();

    printf("Enter your name: ");
    fgets(current_user, sizeof(current_user), stdin);
    current_user[strcspn(current_user, "\n")] = '\0';

    int choice;
    while (1) {
        printf("\nWelcome to NoteBox!\n");
        printf("1. View all notes\n");
        printf("2. Add a new note\n");
        printf("3. Delete one of my notes\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        while (getchar() != '\n'); 

        switch (choice) {
            case 1: view_notes(); break;
            case 2: add_note(); break;
            case 3: delete_note(); break;
            case 4: cleanup_and_exit(0); break;
            default: printf("Invalid choice.\n");
        }
    }
}
