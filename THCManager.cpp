#include "THCManager.h"
#include "THCCommand.h"
#include <iostream>

using namespace ThreadCommand;

THCManager::THCManager() {
	pthread_mutex_init(&mutex, NULL);
}

THCManager::~THCManager() {
	
}

void* THCManager::RunLoop(void* _argument) {
	ThreadCategory* _category = (ThreadCategory*)_argument;
	
	pthread_mutex_lock(&_category->mutex_size);
	while(!_category->list_command.empty()) {
		pthread_mutex_unlock(&_category->mutex_size);
		
		void* _res = NULL;
		if(_category->UpdatePrev)
			_res = _category->UpdatePrev(_category->reference);
		
		pthread_mutex_lock(&_category->mutex_size);
		std::list<THCCommand*>::iterator _it = _category->list_command.begin();
		while (_it != _category->list_command.end()) {
			THCCommand* _command = *_it;
			_it++;
			pthread_mutex_unlock(&_category->mutex_size);
			
			if(_command) {
				if(_command->Command(_res)) {
					
					pthread_mutex_lock(&_category->mutex);
					pthread_mutex_lock(&_category->mutex_size);
					_category->list_command.remove(_command);
					pthread_mutex_unlock(&_category->mutex_size);
					delete _command;
					pthread_mutex_unlock(&_category->mutex);
					
				}
			}
			
			pthread_mutex_lock(&_category->mutex_size);
		}
		pthread_mutex_unlock(&_category->mutex_size);
		
		if(_res)
			free(_res);
		
		if(_category->UpdateNext)
			_category->UpdateNext(_category->reference);
		
		pthread_mutex_lock(&_category->mutex_size);
		
	}
	pthread_mutex_unlock(&_category->mutex_size);
	
	_category->t_id = NULL;
	
	return NULL;
}

THCManager* __thc_manager = NULL;

THCManager* THCManager::Share() {
	if(__thc_manager == NULL)
		__thc_manager = new THCManager();
	return __thc_manager;
}

void THCManager::Update() {
	pthread_mutex_lock(&mutex);
	std::list<ThreadCategory>::iterator it = list_thread.begin();
	while(it != list_thread.end()) {
		pthread_mutex_unlock(&mutex);
		
		if((*it).t_id == NULL) {
			pthread_mutex_lock(&(*it).mutex_size);
			if((*it).list_command.size() > 0) {
				sched_param current_schedparam;
				int current_policy;
				pthread_getschedparam(pthread_self(), &current_policy, &current_schedparam);
				
				pthread_attr_t attr;
				sched_param schedparam;
				
				schedparam.sched_priority = current_schedparam.sched_priority * (*it).priotiry;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#ifndef ANDROID_NDK
				pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
#endif
				pthread_attr_setschedpolicy(&attr, SCHED_RR);
				pthread_attr_setschedparam(&attr, &schedparam);
				
				pthread_create(&(*it).t_id, &attr, THCManager::RunLoop, &(*it));
				pthread_attr_destroy(&attr);
			}
			pthread_mutex_unlock(&(*it).mutex_size);
		}
		
		pthread_mutex_lock(&(*it).mutex);
		pthread_mutex_lock(&(*it).mutex_size);
		std::list<THCCommand*>::iterator ic = (*it).list_command.begin();
		while (ic != (*it).list_command.end()) {
			pthread_mutex_unlock(&(*it).mutex_size);
			(*ic)->Update();
			pthread_mutex_lock(&(*it).mutex_size);
			ic++;
		}
		pthread_mutex_unlock(&(*it).mutex_size);
		pthread_mutex_unlock(&(*it).mutex);
		
		pthread_mutex_lock(&mutex);
		it++;
	}
	pthread_mutex_unlock(&mutex);
}

bool THCManager::AddCommand(std::string _category, THCCommand* _command) {
	pthread_mutex_lock(&mutex);
	std::list<ThreadCategory>::iterator it = list_thread.begin();
	while(it != list_thread.end()) {
		pthread_mutex_unlock(&mutex);
		
		if((*it).category == _category) {
			
			pthread_mutex_lock(&(*it).mutex_size);
			(*it).list_command.push_back(_command);
			pthread_mutex_unlock(&(*it).mutex_size);
			
			return true;
		}
		
		pthread_mutex_lock(&mutex);
		it++;
	}
	pthread_mutex_unlock(&mutex);
	
	return false;
}

bool THCManager::SetCategory(std::string _category, float _priotiry,
							 void* reference,
							 void* (*UpdatePrev)(void*),
							 void* (*UpdateNext)(void*)) {
	std::list<ThreadCategory>::iterator it = list_thread.begin();
	while(it != list_thread.end()) {
		if((*it).category == _category) {
			(*it).priotiry = _priotiry;
			(*it).reference = reference;
			(*it).UpdatePrev = UpdatePrev;
			(*it).UpdateNext = UpdateNext;
			return false;
		}
		it++;
	}
	
	list_thread.push_back((ThreadCategory){_category, _priotiry, std::list<THCCommand*>(), reference, UpdatePrev, UpdateNext, NULL, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER});
	
	return true;
}

THCManager::ThreadCategory THCManager::GetCategoryList(std::string _category) {
	std::list<ThreadCategory>::iterator it = list_thread.begin();
	while(it != list_thread.end()) {
		if((*it).category == _category) {
			return *it;
		}
		it++;
	}
	return (ThreadCategory){std::string(), 0.0, std::list<THCCommand*>(), NULL, NULL, NULL, NULL, NULL, NULL};
}