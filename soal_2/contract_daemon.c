#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/inotify.h>

#define LOG_FILE "work.log"
#define CONTRACT_FILE "contract.txt"

volatile sig_atomic_t running = 1;

void write_log(const char *msg) {
    FILE *f = fopen(LOG_FILE, "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

void get_time_str(char *buffer) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", t);
}

void create_contract(int restored) {
    FILE *f = fopen(CONTRACT_FILE, "w");
    char timebuf[100];
    get_time_str(timebuf);

    if (f) {
        fprintf(f, "A promise to keep going, even when unseen.\n");
        if (restored)
            fprintf(f, "restored at: %s\n", timebuf);
        else
            fprintf(f, "created at: %s\n", timebuf);
        fclose(f);
    }
}

void handle_signal(int sig) {
    write_log("We really weren’t meant to be together");
    running = 0;
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    setsid();
    chdir(".");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main() {
    daemonize();

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    srand(time(NULL));

    create_contract(0);

    int fd = inotify_init();
    int wd = inotify_add_watch(fd, ".", IN_DELETE | IN_MODIFY);

    char buffer[1024];

    while (running) {
        // logging tiap 5 detik
        char *status[] = {"awake", "drifting", "numbness"};
        char logmsg[100];
        sprintf(logmsg, "still working... [%s]", status[rand() % 3]);
        write_log(logmsg);

        // cek event
        int length = read(fd, buffer, sizeof(buffer));
        if (length > 0) {
            struct inotify_event *event = (struct inotify_event *) buffer;

            if (event->len) {
                if (strcmp(event->name, CONTRACT_FILE) == 0) {
                    if (event->mask & IN_DELETE) {
                        sleep(1);
                        create_contract(1);
                    }
                    if (event->mask & IN_MODIFY) {
                        write_log("contract violated.");
                        create_contract(1);
                    }
                }
            }
        }

        sleep(5);
    }

    close(fd);
    return 0;
}
