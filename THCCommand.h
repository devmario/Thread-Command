#ifndef THCCommand_h
#define THCCommand_h

#include <string>
#include "THCEvent.h"
#include "THCManager.h"

namespace ThreadCommand {
	class THCEvent;
	
	class THCCommand {
	protected:
		std::string tag;
		std::string category;
		THCEvent* event;
		bool inited;
		bool completed;
		
	public:
		THCCommand();
		
		virtual ~THCCommand();
		
		std::string GetTag();
		
		std::string GetCategory();
		
		void Regist(std::string _category, std::string _tag, THCEvent* _event);
		
		void UnRegist();
		
		bool Command(void* _ptr = NULL);
		
		virtual void Update();
		
		virtual bool Init(void* _ptr = NULL) = 0;
		virtual bool Run(void* _ptr = NULL) = 0;
		virtual bool Clean(void* _ptr = NULL) = 0;
		
		friend class THCManager;
	};
}

#endif
