#include "THCEvent.h"
#include "THCCommand.h"
#include "THCManager.h"

using namespace ThreadCommand;

THCEvent::THCEvent() {
	
}

THCEvent::~THCEvent() {
	while (list_command.size()) {
		THCCommand* _command = list_command.front();
		_command->UnRegist();
	}
}

THCCommand* THCEvent::FindTagCommand(const std::string _tag) {
	std::list<THCCommand*>::iterator _iterator;
	for(_iterator = list_command.begin(); _iterator != list_command.end(); _iterator++) {
		THCCommand* _command = *_iterator;
		if(_command->GetTag() == _tag) {
			return _command;
		}
	}
	return NULL;
}

void THCEvent::Run(std::string _category, std::string _tag, THCCommand* _command) {
	_command->Regist(_category, _tag, this);
	list_command.push_back(_command);
	THCManager::Share()->AddCommand(_category, _command);
}

bool THCEvent::Stop(std::string _category, std::string _tag) {
	std::list<THCCommand*>::iterator _it = list_command.begin();
	while (_it != list_command.end()) {
		THCCommand* _command = *_it;
		_it++;
		if(_command->GetCategory() == _category && _command->GetTag() == _tag) {
			_command->UnRegist();
		}
	}
	return true;
}

void THCEvent::RemoveCommandList(THCCommand* _command) {
	list_command.remove(_command);
}