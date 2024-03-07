#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

#include <sys/types.h>
#include <sys/wait.h>

using namespace std;


void ctrlCHandler(int sig_num) {
    cout << "smash: got ctrl-C" << endl;
    SmallShell& smash = SmallShell::getInstance();

    if (smash.fg_proccess_pid != -1) {
        // Check if the process is still alive
        int status;
        pid_t result = waitpid(smash.fg_proccess_pid, &status, WNOHANG);

        if (result == 0) {
            // Process is still alive, kill it
            if (kill(smash.fg_proccess_pid, SIGKILL) != 0) {
                perror("smash error: kill failed");
                return;
            }
            cout << "smash: process " << smash.fg_proccess_pid << " was killed" << endl;
        } else if (result == -1) {
            perror("smash error: waitpid failed");
            return;
        }
    }
    smash.fg_proccess_pid = -1;
}
/*void alarmHandler(int sig_num) {  /// bonus
  // TODO: Add your implementation
}*/

