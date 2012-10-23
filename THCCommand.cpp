#include "THCCommand.h"

using namespace ThreadCommand;

THCCommand::THCCommand() {
	event = NULL;
	inited = false;
	completed = false;
}

THCCommand::~THCCommand() {
}

std::string THCCommand::GetTag() {
	return tag;
}

std::string THCCommand::GetCategory() {
	return category;
}

void THCCommand::Regist(std::string _category, std::string _tag, THCEvent* _event) {
	category = _category;
	tag = _tag;
	event = _event;
}

void THCCommand::UnRegist() {
	THCEvent* _event_tmp = event;
	event = NULL;
	if(_event_tmp)
		_event_tmp->RemoveCommandList(this);
}

bool THCCommand::Command(void* _ptr) {
	if(event) {
		if(!completed) {
			if(!inited) {
				if(Init(_ptr)) {
					inited = true;
				}
			} else {
				if(Run(_ptr)) {
					completed = true;
				}
			}
		}
		return false;
	} else {
		return Clean(_ptr);
	}
}

void THCCommand::Update() {
	if(completed) {
		if(event) {
			event->CompleteCommand(category, tag, this);
			UnRegist();
		}
	}
}