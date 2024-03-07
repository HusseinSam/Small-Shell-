#include <unistd.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;
const std::string WHITESPACE = " \n\r\t\f\v";
#if 0
#define FUNC_ENTRY()  \
  cout << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cout << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#define PATH_MAX 260
#endif

inline bool isInteger(const std::string & str)
{
  char * _long;
  if(str.empty() || ((!isdigit(str[0])) && (str[0] != '+') && (str[0] != '-'))) return false;
  strtol(str.c_str(), &_long, 10);
  if(!(*_long)) return true;
  return false;
}
string _ltrim(const std::string& s)
{
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string& s)
{
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string& s)
{
  return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_trim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

bool _isBackgroundComamnd(const char* cmd_line) {
  const string str(cmd_line);
  return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
  const string str(cmd_line);
  // find last character other than spaces
  unsigned int idx = str.find_last_not_of(WHITESPACE);
  // if all characters are spaces then return
  if (idx == string::npos) {
    return;
  }
  // if the command line does not end with & then return
  if (cmd_line[idx] != '&') {
    return;
  }
  // replace the & (background sign) with space and then remove all tailing spaces.
  cmd_line[idx] = ' ';
  // truncate the command line string up to the last non-space character
  cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h

SmallShell::SmallShell() :small_shell_name("smash"),small_shell_pid(getpid()),prev_dir(nullptr)
,fg_proccess_pid(-1),unfinished_jobs(new JobsList()/*,curr_cmd(null)*/){
// TODO: add your implementation

}

/*
SmallShell::~SmallShell() {
// TODO: add your implementation
}
*/

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/



bool contains_Redirection(const std::string& trimmedString)
{
  return (trimmedString.find('>') != std::string::npos || trimmedString.find(">>") != std::string::npos);
}

bool contains_pipe(const std::string& trimmedString)
{
    return (trimmedString.find('|') != std::string::npos || trimmedString.find("|&") != std::string::npos);
}


Command * SmallShell::CreateCommand(const char* cmd_line) {
	 char** arg = new char* [21];
	int num_of_arg = _parseCommandLine(cmd_line,arg);
  if(num_of_arg == 0)
  {
    return nullptr;
  }
        string cmd_s = _trim(string(cmd_line));
        string firstWord = cmd_s.substr(0, cmd_s.find_first_of(" \n"));
        if (contains_Redirection(cmd_s)){
          return new RedirectionCommand(cmd_line);
        }
        if (contains_pipe(cmd_s)){
            return new PipeCommand(cmd_line);
        }
        if (firstWord.compare("chprompt") == 0 || firstWord.compare("chprompt&") == 0) {
          return new ChpromptCommand(cmd_line);
        }
        else if (firstWord.compare("pwd") == 0 || firstWord.compare("pwd&") == 0 ) {
          return new GetCurrDirCommand(cmd_line);
        }
        else if (firstWord.compare("showpid") == 0 || firstWord.compare("showpid&") == 0) {
          return new ShowPidCommand(cmd_line);
        }
        else if (firstWord.compare("cd") == 0 || firstWord.compare("cd&") == 0) { ////
          return new ChangeDirCommand(cmd_line);
        }
        else if (firstWord.compare("jobs") == 0 || firstWord.compare("jobs&") == 0) {
          return new JobsCommand(cmd_line,this->unfinished_jobs);
        }
        else if (firstWord.compare("fg") == 0 || firstWord.compare("fg&") == 0) {
          return new ForegroundCommand(cmd_line,this->unfinished_jobs);
        }
        else if (firstWord.compare("quit") == 0 || firstWord.compare("quit&") == 0) {
          return new QuitCommand(cmd_line,this->unfinished_jobs);
        }
        else if (firstWord.compare("kill") == 0 || firstWord.compare("kill&") == 0) {
            return new KillCommand(cmd_line,this->unfinished_jobs);
        }
        else if (firstWord.compare("chmod") == 0 || firstWord.compare("chmod&") == 0) {
          return new ChmodCommand(cmd_line);
        }
        else {
          return new ExternalCommand(cmd_line);
        }
  return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line, bool redirected , int fd) 
{

  // TODO: Add your implementation here
  // for example:
   Command* cmd = CreateCommand(cmd_line);
   //cmd->execute();
  if(cmd == nullptr)return;

  if(redirected)
  {
    int destination = 1;
    /*if(piped_err)
    {
      dest = 2;
    }*/
    int file_out = dup(destination);

    if(file_out == -1 || dup2(fd,destination) == -1)
    {
      perror("smash error: dup failed");
      //delete cmd;
      //fg_proccess_pid = small_shell_pid;
      //curr_cmd = NULL;
      return;
    }
   

    //fg_proccess_pid = small_shell_pid;
    cmd->execute();

    close(destination);
    if(dup2(file_out,destination) == -1)
    {
      perror("smash error: dup2 failed");
      //delete cmd;
      //fg_proccess_pid = small_shell_pid;
      //curr_cmd = NULL;
      return;
    }
    close(file_out);
    close(fd);
    return;
  }
  //fg_proccess_pid = small_shell_pid;
  cmd->execute();
  //delete cmd;
  //fg_proccess_pid = small_shell_pid;
  //curr_cmd = NULL;

  // Please note that you must fork smash process for some commands (e.g., external commands....)

  

}

/////////////////////////////////////////////////job list///////////////////////////////////////////////////



JobsList::JobEntry::JobEntry(char *cmd_line, int job_id, pid_t pid/*,bool isStopped*/) :
cmd_line(cmd_line) , job_id(job_id),job_pid(pid)/*,isStopped(isStopped)*/
{

}


JobsList::JobsList() :jobs_list() , max_job_id(0)
{

}

void JobsList::addJob(Command* cmd,  pid_t job_pid/*,bool isStopped*/) {

  removeFinishedJobs();
    this->max_job_id++;
  this->jobs_list.push_back(new JobEntry(cmd->get_cmd(),this->max_job_id,job_pid));

}

void JobsList::printJobsList()
{
  //for (JobEntry* job : this->jobs_list)

  for (JobEntry* unfinishedjob : this->jobs_list)
  {
    cout << "["<<unfinishedjob->job_id<<"]"<<" "<<unfinishedjob->cmd_line<<endl;
  }
}


void JobsList::killAllJobs()
{
  //for (JobEntry* job : this->jobs_list)
  //this->removeFinishedJobs();
  for (JobEntry* unfinishedjob : this->jobs_list)
  {
    cout << unfinishedjob->job_pid << ": " <<  unfinishedjob->cmd_line << endl;
    if(kill(unfinishedjob->job_pid, SIGKILL) == -1)
    {
      perror("smash error: kill failed");
        return;
    }
  }

}

void JobsList::removeFinishedJobs() // upfdate max jobid
{
  if (jobs_list.size()==0)
  {
    max_job_id = 0;
    return;
  }

  //remove all finished jobs;
  for (auto unfinishedjob = jobs_list.begin(); unfinishedjob != jobs_list.end(); ++unfinishedjob)
  {
    int job_wait_status = waitpid((*unfinishedjob)->job_pid, nullptr, WNOHANG);
    if ( job_wait_status == (*unfinishedjob)->job_pid || job_wait_status ==-1) // check if son finished
    {
      this->jobs_list.erase(unfinishedjob);
      --unfinishedjob;
    }
  }


  int max_id =0;//update max_job_id for remaining jobs
  for (JobEntry* unfinishedjob : this->jobs_list)
  {
    if(unfinishedjob->job_id > max_id)
    max_id = unfinishedjob->job_id;
  }
  this->max_job_id = max_id;

}

JobsList::JobEntry*  JobsList::getJobById(int jobId)
{
  for (JobsList::JobEntry* unfinishedjob : this->jobs_list)
  {
    if(unfinishedjob->job_id==jobId)
    {
      return unfinishedjob;
    }
  }
  return nullptr;

}

void JobsList::removeJobById(int jobId)
{
  for (auto unfinishedjob = this->jobs_list.begin(); unfinishedjob != this->jobs_list.end(); ++unfinishedjob)
  {
    if ((*unfinishedjob)->job_id == jobId)
    {
      jobs_list.erase(unfinishedjob);
      if(jobId == this->max_job_id)
      {
      //this->max_job_id--;///wrongggggg
          int max_id =0;//update max_job_id for remaining jobs
          for (JobEntry* unfinishedjob : this->jobs_list)
          {
              if(unfinishedjob->job_id > max_id)
                  max_id = unfinishedjob->job_id;
          }
          this->max_job_id = max_id;
      }
      return;
    }
  }

}


JobsList::JobEntry*  JobsList::getLastJob(int* lastJobId)
{
  *lastJobId = this->max_job_id;
  return getJobById(max_job_id);

}



/*JobEntry* JobList::getLastStoppedJob(int *jobId)
{}*/

///////////////////////////////////////////////commands///////////////////////////////////////////////////////

Command::Command(const char *cmd_line) {
  this->cmd_line = new char[COMMAND_ARGS_MAX_LENGTH];
  strcpy(this->cmd_line,cmd_line);
  this->arg = new char*[COMMAND_MAX_ARGS];
  this->num_of_arg = _parseCommandLine(cmd_line , this->arg);

}

BuiltInCommand::BuiltInCommand(const char *cmd_line) : Command(cmd_line)
{
  if( _isBackgroundComamnd(cmd_line))
  {
    _removeBackgroundSign(this->cmd_line);
    this->num_of_arg = _parseCommandLine(cmd_line , this->arg);
  }

}
////////////////////////////////////////implimintation of built in commands//////////////////////////////////////


void ChpromptCommand::execute()
{
  SmallShell& smash = SmallShell::getInstance(); ///keep this
    smash.unfinished_jobs->removeFinishedJobs();
  if(num_of_arg == 1)
  {
    smash.small_shell_name = "smash";
  }else
  {
    smash.small_shell_name = arg[1];
  }

}

void ShowPidCommand::execute()
{
  SmallShell &shell = SmallShell::getInstance();

    shell.unfinished_jobs->removeFinishedJobs();
  std::cout << "smash pid is " << shell.small_shell_pid << std::endl;
}

void GetCurrDirCommand::execute() // char *getcwd(char buf[.size], size_t size); // PATH_MAX file system limits
{
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

  char* CurrDir= new char[PATH_MAX];
  if(!getcwd(CurrDir , PATH_MAX))
  {
    perror("smash error: getcwd failed");
    delete[] CurrDir;
    return;

  }
  cout << CurrDir << endl;
  delete[] CurrDir;
}


void ChangeDirCommand::execute()
/// int chdir(const char *path);
/// changes the current working directory of the calling process to the directory specified in path.
{
    SmallShell& smash = SmallShell::getInstance(); /// keep this
    smash.unfinished_jobs->removeFinishedJobs();

  if(num_of_arg > 2)
  {
    cerr << "smash error: cd: too many arguments" << endl;
    return;
  }
  else if(strcmp(arg[1],"-")==0)//cd- called
  {
    if (smash.prev_dir==nullptr) //if dir_history is empty
    {
      cerr << "smash error: cd: OLDPWD not set" << endl;
      return;
    }

    // current path into the dir_history + chdir

    char *curr_dir = new char[PATH_MAX];
    if(getcwd(curr_dir , PATH_MAX) == NULL)
    {
      perror("smash error: getcwd failed");
      delete[] curr_dir;
      return;
    }
    if(chdir(smash.prev_dir)==-1)// changing the cur dir to be the last dir
    {
      perror("smash error: chdir failed");
      delete[] curr_dir;
      return;
    }
    if(smash.prev_dir) // delete the last dir from history
    {
      delete[] smash.prev_dir;
    }


    smash.prev_dir = new char[PATH_MAX];
    strcpy(smash.prev_dir , curr_dir); // changing the last dir to be the curr before changing
    delete[] curr_dir;
    return;

  }
  else
  {
    //putting the curr path into the shell->last_dir and then change the path using chdir
    char *curr_dir = new char[PATH_MAX];
    if(getcwd(curr_dir , PATH_MAX) == NULL)
    {
      perror("smash error: getcwd failed");
      delete[] curr_dir;
      return;
    }
    if(chdir(arg[1])==-1)// changing the cur dir to be the given path that is saved at arg[1]
    {
      perror("smash error: chdir failed");
      return;
    }

   if(smash.prev_dir)
   {
      delete smash.prev_dir;
   }
   smash.prev_dir = new char[PATH_MAX];
   strcpy(smash.prev_dir , curr_dir); // changing the last dir to be the curr before changing
   delete[] curr_dir;
  }
}

void JobsCommand::execute() //jobs command prints the unfinished jobs list
{
    jobs->removeFinishedJobs();
  jobs->printJobsList();
}

void ForegroundCommand::execute(){
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

    JobsList::JobEntry* fg_job;
    if ( num_of_arg >= 2 )
    {
        if ( isInteger(arg[1]))
        {
            fg_job = jobs->getJobById(std::stoi(arg[1]));
            if(fg_job==nullptr)
            {
                cerr<<"smash error: fg: job-id "<< arg[1] << " does not exist" << endl;
                return;
            }
        }
        if(num_of_arg > 2 || !isInteger(arg[1]) )
        {
            cerr << "smash error: fg: invalid arguments"<< endl;
            return;
        }


    }else{
        if(jobs->jobs_list.size()==0) {
            cerr << "smash error: fg: jobs list is empty"<< endl;
            return;
        }
        fg_job = jobs->getJobById(jobs->max_job_id);
        if(fg_job==nullptr)
        {
            cerr<<"smash error: fg: job-id "<< jobs->max_job_id << " does not exist" << endl;
            return;
        }
    }

    cout << fg_job->cmd_line << " " << fg_job->job_pid << endl;
    SmallShell::getInstance().fg_proccess_pid = fg_job->job_pid;
    //strcpy( SmallShell::getInstance().fg_cmd_line,job->cmd_line);

    if(waitpid(fg_job->job_pid,nullptr,WUNTRACED)==-1)
    {
        perror("smash error: waitpid failed");
        return;
    }
    SmallShell::getInstance().fg_proccess_pid =-1;
    jobs->removeJobById(fg_job->job_id);

}


void QuitCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

    if(num_of_arg>1 && !strcmp(arg[1],"kill"))
    {

      std::cout<<"smash: sending SIGKILL signal to "<< jobs->jobs_list.size() <<" jobs:"<<std::endl;
      ///for tests
      //jobs->printJobsList();
      ///
      jobs->killAllJobs();
    }
    delete this;
    exit(0);
}

void KillCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

  if(num_of_arg >=3 && isInteger(arg[2])    ) {
      JobsList::JobEntry *job_entry = jobs->getJobById(std::stoi(arg[2]));
      if (job_entry == nullptr) {
          cerr << "smash error: kill: job-id " << arg[2] << " does not exist" << endl;
          return;
      }
      if(num_of_arg !=3 || !isInteger(arg[1]) || !isInteger(arg[2]) || stoi(arg[1])>= 0 ){
          std::cerr << "smash error: kill: invalid arguments" << endl;
          return;
      }

          int sig = -(stoi(arg[1]));
          if (kill(job_entry->job_pid, -stoi(arg[1])) == -1) {
              perror("smash error: kill failed");
              return;
          }

          if (sig == SIGKILL || sig == SIGTERM || sig == SIGQUIT || sig == SIGINT) {
              jobs->removeJobById(job_entry->job_id);
          }
          cout << "signal number " << sig << " was sent to pid " << job_entry->job_pid << endl;

  }else {
      std::cerr << "smash error: kill: invalid arguments" << endl;
      return;
  }
}



////////////////////////////////////////// External command /////////////////////////////////////////
ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {
    strcpy(cmd_line_copy,string(cmd_line).c_str());

    is_background = _isBackgroundComamnd(cmd_line);
    if (is_background) {
        _removeBackgroundSign(cmd_line_copy);
        this->num_of_arg = _parseCommandLine(cmd_line_copy, this->arg);
    }
}

bool is_complex(const char *cmd_line)
{

  if (strchr(cmd_line, '*') || strchr(cmd_line, '?'))
  {
    return true;
  }
  return  false;
}

void ExternalCommand::execute()
{
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();
  ///// decide if its a simple command or complex command
  ///// fork
  //////////exec
  bool complex= is_complex(cmd_line);
	
  pid_t pid = fork();
  if (pid == -1)
  {  //if fork fails
    perror("smash error: fork failed");
    return;
  }
  if (pid == 0) /// son
  {
      //setpgrp
      if (setpgrp() == -1) {
          perror("smash error: setpgrp failed");
          return;
      }

    if(complex)
    {
        if (execvp("bin/bash", const_cast<char* const*>(new const char*[4]{"bin/bash", "-c", cmd_line, nullptr})) == -1)
        {
          perror("smash error: execvp failed");
            exit(EXIT_FAILURE); // Terminate child with error status

        }

    }
    else
    {
        if(execvp(arg[0],arg)==-1) //// change for simple command
        {
          perror("smash error: execvp failed");
            exit(EXIT_FAILURE); // Terminate child with error status

        }

    }
  }
  if (pid != 0)
  {
        if(is_background)
        {
			//cout << "we are __ in back"<<endl;
          SmallShell::getInstance().unfinished_jobs->addJob(this,pid);
        }
        else
        {
			//cout << "we are not in back"<<endl;
          SmallShell::getInstance().fg_proccess_pid = pid;
          //strcpy(SmallShell::getInstance().fg_cmd_line,cmd_line);
            int status;
            if (waitpid(pid, &status, WUNTRACED) == -1)
            {
                SmallShell::getInstance().fg_proccess_pid = -1;
                perror("smash error: waitpid failed");
                return;
            }
          SmallShell::getInstance().fg_proccess_pid = -1;
        }

  }
}

char* cutRedirectionCommand(char** arg, int num_of_arg)
{
  int new_cmd_length = 0;
  for (int i=0; i<num_of_arg-2; i++)
  {
    new_cmd_length+=strlen(arg[i])+1;
  }

  char* new_cmd = new char[new_cmd_length];
  for(int i=0; i<num_of_arg-2; i++)
  {
    strcat(new_cmd,arg[i]);
    if(i!= num_of_arg-3){
      strcat(new_cmd," ");
    }
  }
  return new_cmd;
}
void RedirectionCommand::execute()////
{
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

  int new_fd;
  if(strcmp(arg[1],">")==0)
  {
    new_fd=open(arg[2],O_RDWR|O_CREAT|O_TRUNC,0644);
    if(new_fd==-1)
    {
      perror("smash error: open failed");
      return;
    }
    }
    else if (strcmp(arg[1],">>")==0)
    {
    new_fd=open(arg[2],O_RDWR|O_CREAT|O_APPEND,0655);
    if(new_fd==-1)
    {
      perror("smash error: open failed");
      return;
    }
  }
  SmallShell& instance_of_smash = SmallShell::getInstance();
  char* new_cmd = cutRedirectionCommand(arg, num_of_arg);
  instance_of_smash.executeCommand(new_cmd, true, new_fd);
}

void ChmodCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

    if (num_of_arg != 3 || isInteger(arg[1])==0 )
    {
      cerr << "smash error: chmod: invalid aruments" << endl;
      return;
    }
    const char* file_path = arg[2];
    mode_t new_file_mode = (mode_t) strtol(arg[1], NULL, 8);
    if (chmod(file_path, new_file_mode) == -1) {
      perror("smash error: chmod failed");
      return;
    }
}


PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line) {


    string my_str=string (cmd_line);
    size_t index = my_str.find("|&");
    if(index == std::string::npos) { // "|"
        int no_ampersand_index = string(cmd_line).find("|");
        this->no_ampersand = true;
        strcpy(this->first_command, _trim(my_str.substr(0, no_ampersand_index)).c_str());
        strcpy(this->second_command, _trim(my_str.substr(no_ampersand_index+1, my_str.size() - no_ampersand_index - 1)).c_str());
    } else{
        this->no_ampersand = false;
        strcpy(this->first_command, _trim(my_str.substr(0, index)).c_str());
        strcpy(this->second_command , _trim(my_str.substr(index+2, my_str.size() - index - 1)).c_str());
    }
}

void PipeCommand::execute() {
    SmallShell& smash = SmallShell::getInstance();
    smash.unfinished_jobs->removeFinishedJobs();

    int fd[2]; //fd[0] read , fd[1] write
    pipe(fd);
    pid_t first_child = fork();// first child for the write
    if (first_child == -1) { // check if fork success
        perror("smash error: fork failed");
        if (close(fd[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(fd[1]) == -1) {
            perror("smash error: close failed");
        }
        return;
    }
    if (first_child == 0) { //first son
        if (setpgrp() == -1) { //check if setpgrp success
            perror("smash error: setpgrp failed");
            if (close(fd[0]) == -1) {
                perror("smash error: close failed");
            }
            if (close(fd[1]) == -1) {
                perror("smash error: close failed");
            }
            return;
        }
        if (no_ampersand) {
            if (dup2(fd[1], 1) == -1) {//fd[1] is in stdout now
                perror("smash error: dup2 failed");
                if (close(fd[0]) == -1) {// close fd[0] cause we want to write from this child not read
                    perror("smash error: close failed");
                }
                if (close(fd[1]) == -1) {// close fd[1] cause we duplicated it to stdout
                    perror("smash error: close failed");
                }
                return;
            }
        } else {
            if (dup2(fd[1], 2) == -1) {//fd[1] is in stderr now
                perror("smash error: dup2 failed");
                if (close(fd[0]) == -1) {// close fd[0] cause we want to write from this child not read
                    perror("smash error: close failed");
                }
                if (close(fd[1]) == -1) {// close fd[1] cause we duplicated it to stderr
                    perror("smash error: close failed");
                }
                return;
            }
        }
        if (close(fd[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(fd[1]) == -1) {
            perror("smash error: close failed");
        }
        SmallShell::getInstance().executeCommand(first_command);
        exit(0);
    }
    if (first_child != 0) {
        if (waitpid(first_child, nullptr, WUNTRACED) == -1) {
            perror("smash error: waitpid failed");
            return;
        }
    }

    pid_t second_child = fork(); // second child for the read
    if (second_child == -1) {
        perror("smash error: fork failed");
        if (close(fd[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(fd[1]) == -1) {
            perror("smash error: close failed");
        }
        return;
    }
    if (second_child == 0) { //second son
        if (setpgrp() == -1) { //check if setpgrp success
            perror("smash error: setpgrp failed");
            if (close(fd[0]) == -1) {
                perror("smash error: close failed");
            }
            if (close(fd[1]) == -1) {
                perror("smash error: close failed");
            }
            return;
        }
        if (dup2(fd[0], 0) == -1) {///fd[1] is in stdin now
            perror("smash error: dup2 failed");
            if (close(fd[0]) == -1) {// close fd[0] cause we duplicated it to stdin
                perror("smash error: close failed");
            }
            if (close(fd[1]) == -1) {// close fd[1] cause we want to read from this child not write
                perror("smash error: close failed");
            }
            return;
        }
        if (close(fd[0]) == -1) {
            perror("smash error: close failed");
        }
        if (close(fd[1]) == -1) {
            perror("smash error: close failed");
        }
        SmallShell::getInstance().executeCommand(second_command);
        exit(0);
    }
    if (close(fd[0]) == -1) {
        perror("smash error: close failed");
    }
    if (close(fd[1]) == -1) {
        perror("smash error: close failed");
    }
    if(second_child!=0)
    {
        if (waitpid(second_child,nullptr, WUNTRACED) == -1) {
            perror("smash error: waitpid failed");
            return;
        }
    }

}
