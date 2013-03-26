### how to (example)

+ not tested code
+ no error at my project(not use example)

```c++
#include "THCCommand.h"

using namespace ThreadCommand;

//THCCommand have retain count likely objective - c
class UserCommand : public THCCommand {
public:

  virtual bool Init(void* _ptr) {
    sleep(1);
    std::cout << "Init " << pthread_self() << "\n";
    //if return false recall
    //if return true dont call this function again
    return true;
  }
  
  virtual bool Run(void* _ptr) {
    sleep(1);
    std::cout << "Run " << pthread_self() << "\n";
    //if return false recall
    //if return true dont call this function again
    //first call raise at Init function return true
    return true;
  }
  
  virtual bool Clean(void* _ptr) {
    sleep(1);
    std::cout << "Clean " << pthread_self() << "\n";
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
    //if you want thread count, more regist category with another thread_id
    THCManager::Share()->SetCategory("thread_id", 0.1, NULL, NULL, NULL);
  }
  
  virtual ~UserEvent() {
    //if destruct, delete registed command
  }
  
  //tag is command user id
  virtual void CompleteCommand(std::string _category, std::string _tag, THCCommand* _command) {
    UserCommand* _user_command = (UserCommand*)_command;
    //raise at main thread
    std::cout << "CompleteCommand " << pthread_self() << "\n";
  }
}

//run in main thread please
void test(void) {
  UserEvent event;
  //regist command with thread_id
  //user_command is tag about command
  event.Run("thread_id", "user_command", new UserCommand());
  while(true) {
    //CompleteCommand function caller
    THCManager::Share()->Update();
    sleep(1);
    std::cout << "main loop " << pthread_self() << "\n";
  }
}

```

### info

+ one thread is one category
+ able to one THCEvent controll many thread
+ thread(category) is just thread, THCCommand is queue
+ THCManager::Share()->Update() is callback(raise at THCCommand Clean, destruct)
