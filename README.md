#include "THCCommand.h"
#include <iostream>

using namespace ThreadCommand;

class Command;
	
class Event : public THCEvent {
public:
	Event() {
		//인자들
		//1.생성할 스레드 이름...이미생성된 스레드가 있으면 거기서 돌아간다
		//2.cpu점유율(1.0하면 메인스레드랑 똑같은 점유율로 돌아간다)
		//3.쓰레드콜백에 전해줄 포인터
		//4.쓰레드콜백1
		//5.쓰레드콜백2
		THCManager::Share()->SetCategory("thread_tag", 0.1, NULL, NULL, NULL);
	}
	virtual ~Event() {
	}
	
	void Start() {
		//1.돌릴쓰레드
		//2.커맨드에대한 식별자
		//3.커맨드
		Run("thread_tag", _tag, new Command(0));
	}
	
	//이놈은 command가 다 돌아가고 나서 호출됨
	virtual void CompleteCommand(std::string _category, std::string _tag, THCCommand* _command) {
		//요놈은 메인쓰레드에서호출됨
		std::cout << "complete\n";
		std::cout << ()->count << "\n";
	}
};
	
class Command : public THCCommand {
public:
	int count;

	Command(int _count) {
		std::cout << "new command\n";
		count = _count;
	}
	virtual ~Command() {
		std::cout << "delete command\n";
	}
	
	//트루를 리턴하기전까지 다음3개의 메소드는 계속 실행됨
	//다음의 3개는 다른쓰레드에서 실행됨
	//clean이 true를 리턴하는 시점에서 인스턴스는 메모리에서 내려감
	
	//초기화에 쓰세요
	virtual bool Init(void* _ptr = NULL) {
		std::cout << "init\n";
		return true;
	}
	
	//쓰레드 실행에 쓰세요(true를 리턴하면 Event의 CompleteCommand가 호출됩니다)
	virtual bool Run(void* _ptr = NULL) {
		std::cout << "run\n";
		count++;
		usleep(100000);
		return count > 1000;
	}
	
	//메모리에서 Run을 돌리기 위해 올렷던 자원들을 돌려주는데 쓰세요(위의 함수가 true를 리턴한다음 실행됨)
	virtual bool Clean(void* _ptr = NULL) {
		std::cout << "clean\n";
		return true;
	}
};

int main() {
	Event* event = new Event();
	event->Start();
	while(true) {
		//이 함수가 호출될때 Event의 CompleteCommand를 호출하게 됩니다.
		THCManager::Share()->Update();
		std::cout << "sleep";
		sleep(1);
	}
}
