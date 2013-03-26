### how to

```c++
#include "THCCommand.h"

using namespace ThreadCommand;

//THCCommand have retain count likely objective - c
class UserCommand : public THCCommand {
public:

  virtual bool Init(void* _ptr) {
    //if return false recall
    //if return true dont call this function again
    return true;
  }
  
  virtual bool Run(void* _ptr) {
    //if return false recall
    //if return true dont call this function again
    //first call raise at Init function return true
    return true;
  }
  
  virtual bool Clean(void* _ptr) {
    //if return false recall
    //if return true dont call this function again, destruct class
    //first call raise at Run function return true
    return true;
  }
}

class UserEvent : public THCEvent {
public:
  UserEvent() {
    //thread_id, priority, ...
    THCManager::Share()->SetCategory("thread_id", 0.1, NULL, NULL, NULL);
  }
  
  virtual ~UserEvent() {
    //if destruct, delete registed command
  }
  
  virtual void CompleteCommand(std::string _category, std::string _tag, THCCommand* _command) {
    UserCommand* _user_command = (UserCommand*)_command;
    //raise at main thread
  }
}

//run in main thread please
void test(void) {
  
}

```
