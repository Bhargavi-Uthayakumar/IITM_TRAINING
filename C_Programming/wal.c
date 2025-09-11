#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define WAL_FILE "wal.log"
#define DB_FILE  "db.txt"
#define BUF_SIZE 1024
#define MAX_KEYS 100

static int transaction_id = 1;

typedef struct {
    char key[BUF_SIZE];
    char value[BUF_SIZE];
} KVPair;

static KVPair kv_store[MAX_KEYS];
static int kv_count = 0;

/* ----------------- Utility Functions ----------------- */

int find_kv_index(const char *key) {
    for (int i = 0; i < kv_count; i++) {
        if (strcmp(kv_store[i].key, key) == 0) return i;
    }
    return -1;
}

void store_kv(const char *key, const char *value) {
    int idx = find_kv_index(key);
    if (idx >= 0) {
        strncpy(kv_store[idx].value, value, BUF_SIZE - 1);
        kv_store[idx].value[BUF_SIZE - 1] = '\0';
    } else {
        if (kv_count >= MAX_KEYS) {
            fprintf(stderr, "Too many keys\n");
            exit(1);
        }
        strncpy(kv_store[kv_count].key, key, BUF_SIZE - 1);
        strncpy(kv_store[kv_count].value, value, BUF_SIZE - 1);
        kv_store[kv_count].key[BUF_SIZE - 1] = '\0';
        kv_store[kv_count].value[BUF_SIZE - 1] = '\0';
        kv_count++;
    }
}

void write_file_append(const char *filename, const char *data, int do_fsync) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) 
    { 
        perror("open"); 
        exit(1); 
    }

    ssize_t len = strlen(data);
    if (write(fd, data, len) != len) 
    { 
        perror("write"); 
        close(fd); 
        exit(1); 
    }

    if (do_fsync && fsync(fd) < 0) 
    { 
        perror("fsync"); 
        close(fd); 
        exit(1); 
    }

    close(fd);
}

void sync_data(int do_fsync) {  
    int fd = open(DB_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) 
    { 
        perror("open"); 
        exit(1); 
    }

    char buf[BUF_SIZE];
    for (int i = 0; i < kv_count; i++) {
        int len = snprintf(buf, sizeof(buf), "%s=%s\n", kv_store[i].key, kv_store[i].value);
        if (write(fd, buf, len) != len) 
        { 
            perror("write"); 
            close(fd); 
            exit(1); 
        }
    }

    if (do_fsync && fsync(fd) < 0)
    { 
        perror("fsync db"); 
        close(fd); 
        exit(1); 
    }
    close(fd);
}

int get_next_transaction_id() {
    int fd = open(WAL_FILE, O_RDONLY);
    if (fd < 0) return 1;

    int max_tid = 0;
    char buf[BUF_SIZE], line[BUF_SIZE];
    int line_pos = 0;
    ssize_t n;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                line[line_pos] = '\0';
                if (strncmp(line, "TRANSACTION", 11) == 0) {
                    int tid;
                    if (sscanf(line, "TRANSACTION %d", &tid) == 1)
                        if (tid > max_tid) max_tid = tid;
                }
                line_pos = 0;
            } else if (line_pos < BUF_SIZE - 1) {
                line[line_pos++] = buf[i];
            }
        }
    }

    if (line_pos > 0) {
        line[line_pos] = '\0';
        if (strncmp(line, "TRANSACTION", 11) == 0) {
            int tid;
            if (sscanf(line, "TRANSACTION %d", &tid) == 1)
                if (tid > max_tid) max_tid = tid;
        }
    }

    close(fd);
    return max_tid + 1;
}

void load_existing_db() {
    int fd = open(DB_FILE, O_RDONLY);
    if (fd < 0) return;

    char buf[BUF_SIZE], line[BUF_SIZE];
    int line_pos = 0;
    ssize_t n;

    kv_count = 0;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                line[line_pos] = '\0';
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    store_kv(line, eq + 1);
                }
                line_pos = 0;
            } else if (line_pos < BUF_SIZE - 1) {
                line[line_pos++] = buf[i];
            }
        }
    }

    if (line_pos > 0) {
        line[line_pos] = '\0';
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            store_kv(line, eq + 1);
        }
    }

    close(fd);
}

/* ----------------- Command Functions ----------------- */

void do_write(const char *key, const char *value, int do_fsync) {
    char buf[BUF_SIZE];
    int tid = transaction_id++;

    snprintf(buf, sizeof(buf), "TRANSACTION %d BEGIN\n", tid);
    write_file_append(WAL_FILE, buf, do_fsync);

    snprintf(buf, sizeof(buf), "SET %s %s\n", key, value);
    write_file_append(WAL_FILE, buf, do_fsync);

    snprintf(buf, sizeof(buf), "TRANSACTION %d COMMIT\n", tid);
    write_file_append(WAL_FILE, buf, do_fsync);

    store_kv(key, value);
    sync_data(do_fsync);  

    printf("Wrote (%s=%s) with%s fsync\n", key, value, do_fsync ? "" : "out");
}

void simulate_crash_after_wal(const char *key, const char *value) {
    char buf[BUF_SIZE];
    int tid = transaction_id++;

    snprintf(buf, sizeof(buf), "TRANSACTION %d BEGIN\n", tid);
    write_file_append(WAL_FILE, buf, 1);

    snprintf(buf, sizeof(buf), "SET %s %s\n", key, value);
    write_file_append(WAL_FILE, buf, 1);

    snprintf(buf, sizeof(buf), "TRANSACTION %d COMMIT\n", tid);
    write_file_append(WAL_FILE, buf, 1);

    printf("Simulated crash AFTER WAL (before DB apply)\n");
    exit(1);
}

void recover_from_wal() {
    int fd = open(WAL_FILE, O_RDONLY);
    if (fd < 0) {
        printf("No WAL file to recover from\n");
        return;
    }

    load_existing_db();

    char buf[BUF_SIZE], line[BUF_SIZE];
    int line_pos = 0;
    ssize_t n;

    int current_tid = 0, in_txn = 0;
    char cur_key[BUF_SIZE] = "", cur_val[BUF_SIZE] = "";

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for (int i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                line[line_pos] = '\0';

                if (strncmp(line, "TRANSACTION", 11) == 0) {
                    if (strstr(line, "BEGIN")) {
                        sscanf(line, "TRANSACTION %d BEGIN", &current_tid);
                        in_txn = 1;
                        cur_key[0] = cur_val[0] = '\0';
                    } else if (strstr(line, "COMMIT")) {
                        int tid;
                        sscanf(line, "TRANSACTION %d COMMIT", &tid);
                        if (in_txn && tid == current_tid && cur_key[0] != '\0')
                            store_kv(cur_key, cur_val);
                        in_txn = 0;
                    }
                } else if (in_txn && strncmp(line, "SET ", 4) == 0) {
                    sscanf(line + 4, "%s %s", cur_key, cur_val);
                }

                line_pos = 0;
            } else if (line_pos < BUF_SIZE - 1) {
                line[line_pos++] = buf[i];
            }
        }
    }

    close(fd);
    sync_data(1);   
    printf("Recovery done. Applied committed transactions.\n");
}

void display_state() {
    printf("---- WAL LOG ----\n");
    int fd = open(WAL_FILE, O_RDONLY);
    if (fd >= 0) {
        char buf[BUF_SIZE];
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf))) > 0)
            write(STDOUT_FILENO, buf, n);
        close(fd);
    } else {
        printf("(empty)\n");
    }

    printf("\n---- DB FILE ----\n");
    fd = open(DB_FILE, O_RDONLY);
    if (fd >= 0) {
        char buf[BUF_SIZE];
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf))) > 0)
            write(STDOUT_FILENO, buf, n);
        close(fd);
    } else {
        printf("(empty)\n");
    }
}

/* ----------------- Main ----------------- */

int main(int argc, char *argv[]) {
    load_existing_db();
    transaction_id = get_next_transaction_id();

    if (argc < 2) {
        fprintf(stderr,
            "Usage:\n"
            "  %s write <key> <value>\n"
            "  %s write-nosync <key> <value>\n"
            "  %s crash-after-wal <key> <value>\n"
            "  %s recover\n"
            "  %s display\n",
            argv[0], argv[0], argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "write") == 0 && argc == 4) {
        do_write(argv[2], argv[3], 1);
    } else if (strcmp(argv[1], "write-nosync") == 0 && argc == 4) {
        do_write(argv[2], argv[3], 0);
    } else if (strcmp(argv[1], "crash-after-wal") == 0 && argc == 4) {
        simulate_crash_after_wal(argv[2], argv[3]);
    } else if (strcmp(argv[1], "recover") == 0) {
        recover_from_wal();
    } else if (strcmp(argv[1], "display") == 0) {
        display_state();
    } else {
        fprintf(stderr, "Unknown command\n");
        return 1;
    }

    return 0;
}
