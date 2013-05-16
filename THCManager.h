#ifndef THCManager_h
#define THCManager_h

#include <list>
#include <pthread.h>
#include <string>

namespace ThreadCommand {
	class THCCommand;
	
	class THCManager {
	public:
		//list_thread 락
		pthread_mutex_t mutex;
		
		typedef struct ThreadCategory {
			std::string category;
			float priotiry;
			
			std::list<THCCommand*> list_command;
			
			void* reference;
			void* (*UpdatePrev)(void*);
			void* (*UpdateNext)(void*);
			
			pthread_t t_id;
			//THCCommand 락
			pthread_mutex_t mutex;
			//list_command의 사이즈 락
			pthread_mutex_t mutex_size;
		} ThreadCategory;
		
	private:
		THCManager();
		~THCManager();
		
		std::list<ThreadCategory> list_thread;
		
		static void* RunLoop(void*);
		
	public:
		static THCManager* Share();
		
		void Update();
		
		bool AddCommand(std::string _category, THCCommand* _command);
		
		bool SetCategory(std::string _category, float _priotiry,
						 void* reference,
						 void* (*UpdatePrev)(void*),
						 void* (*UpdateNext)(void*));
		
		ThreadCategory GetCategoryList(std::string _category);
		
		friend class THCCommand;
	};
}

#endif
