#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
protected:
    char* cmd_line;
    char** arg;
    int num_of_arg;
 public:
  Command(const char* cmd_line);
  virtual ~Command() = default;
  virtual void execute() = 0;
  //virtual void prepare();
  //virtual void cleanup();
  // TODO: Add your extra methods if needed
  char* get_cmd(){
      return cmd_line;
  }
};

class BuiltInCommand : public Command {
 public:
  explicit BuiltInCommand(const char* cmd_line);
  virtual ~BuiltInCommand()=default;
};

class ExternalCommand : public Command {
    char cmd_line_copy[COMMAND_ARGS_MAX_LENGTH] = "";
    bool is_background;
 public:
  ExternalCommand(const char* cmd_line);
  virtual ~ExternalCommand()= default;
  void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
    char first_command[COMMAND_ARGS_MAX_LENGTH];
    char second_command[COMMAND_ARGS_MAX_LENGTH];
    bool no_ampersand;
public:
    explicit PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};

class RedirectionCommand : public Command {
 // TODO: Add your data members
 public:
  explicit RedirectionCommand(const char* cmd_line): Command(cmd_line){};
  virtual ~RedirectionCommand()=default;
  void execute() override;
  //void prepare() override;
  //void cleanup() override;
};


class ChangeDirCommand : public BuiltInCommand {  /// cd command
// TODO: Add your data members 
public:
  ChangeDirCommand(const char* cmd_line):BuiltInCommand(cmd_line){};
  virtual ~ChangeDirCommand()=default;
  void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {   
 public:
  GetCurrDirCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
  virtual ~GetCurrDirCommand()=default;
  void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
 public:
  ShowPidCommand(const char* cmd_line): BuiltInCommand(cmd_line){};
  virtual ~ShowPidCommand()=default;
  void execute() override;
};

class JobsList;
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members 
public:
 JobsList* jobs;
  QuitCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line),jobs(jobs){};
  virtual ~QuitCommand() =default;
  void execute() override;
};

class ChpromptCommand : public BuiltInCommand {
public:
    explicit ChpromptCommand(const char *cmd_line):BuiltInCommand(cmd_line){};

    virtual  ~ChpromptCommand() = default;

    void execute() override;
};


/// ////////////////////////////////////////////////////////////////////////////////////////////////////


class JobsList {
 
 public:
  class JobEntry 
  {
   // TODO: Add your data members
   public:
   char* cmd_line;
   int job_id;
   pid_t  job_pid; 
   //bool isStopped;
   JobEntry( char* cmd_line , int job_id ,pid_t  pid);
   ~JobEntry() = default;

  };
 // TODO: Add your data members

 std::vector<JobEntry*> jobs_list;
 int max_job_id;

 public:
  JobsList();
  ~JobsList() = default;
  void addJob(Command* cmd, pid_t job_pid/*,bool isStopped = false*/);
  void printJobsList();
  void killAllJobs();
  void removeFinishedJobs();
  JobEntry * getJobById(int jobId);
  void removeJobById(int jobId);
  JobEntry * getLastJob(int* lastJobId);
  //JobEntry *getLastStoppedJob(int *jobId);
  // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
 // TODO: Add your data members
 JobsList* jobs;
 public:
  JobsCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),jobs(jobs){};
  virtual ~JobsCommand() =default;
  void execute() override;
};

class KillCommand : public BuiltInCommand {
 // TODO: Add your data members
 JobsList* jobs;
 public:
  KillCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),jobs(jobs){};
  virtual ~KillCommand() =default;
  void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
 // TODO: Add your data members
 JobsList* jobs;
 public:
  ForegroundCommand(const char* cmd_line, JobsList* jobs):BuiltInCommand(cmd_line),jobs(jobs){}
  virtual ~ForegroundCommand() =default;
  void execute() override;
};

class ChmodCommand : public BuiltInCommand {
 public:
  ChmodCommand(const char* cmd_line):BuiltInCommand(cmd_line){};
  virtual ~ChmodCommand() =default;
  void execute() override;
};


////////////////////////////////////////////////////////////////////////////////////////////

class SmallShell {
 private:
  // TODO: Add your data members
  SmallShell() ;
 public:
    // data members
  std::string small_shell_name;
  pid_t small_shell_pid;
  char* prev_dir;
  pid_t fg_proccess_pid;
  JobsList* unfinished_jobs;
  //Command* curr_cmd;

  Command *CreateCommand(const char* cmd_line);
  SmallShell(SmallShell const&)      = delete; // disable copy ctor
  void operator=(SmallShell const&)  = delete; // disable = operator
  static SmallShell& getInstance() // make SmallShell singleton
  {
    static SmallShell instance; // Guaranteed to be destroyed.
    // Instantiated on first use.
    return instance;
  }
  ~SmallShell() =default;
  void executeCommand(const char* cmd_line, bool redirected = false, int fd = -1);
  // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
