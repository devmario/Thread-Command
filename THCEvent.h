#ifndef THCEvent_h
#define THCEvent_h

#include <string>
#include <list>

namespace ThreadCommand {
	class THCCommand;
	
	class THCEvent {
	protected:
		std::list<THCCommand*> list_command;
		
		THCCommand* FindTagCommand(const std::string _tag);
		
	public:
		THCEvent();
		virtual ~THCEvent();
		
		void Run(std::string _category, std::string _tag, THCCommand* _command);
		bool Stop(std::string _category, std::string _tag);
		
		void RemoveCommandList(THCCommand* _command);
		
		virtual void CompleteCommand(std::string _category, std::string _tag, THCCommand* _command) = 0;
	};
}

#endif
