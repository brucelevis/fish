#ifndef EVENTPOLLER_H
#define EVENTPOLLER_H

#include <map>
#include <vector>
#include <queue>
#include "../time/TimerWheel.h"
#include "../time/TimerHeap.h"
#include "../time/TimerEngineT.h"
class TimeoutHandler;

namespace Network 
{
	class InputHandler;
	class OutputHandler;
	class ErrorHandler;

	typedef std::vector<int>				IDMap;
	typedef std::vector<InputHandler*>		ReadHandles;
	typedef std::vector<OutputHandler*>		WriteHandles;
	typedef std::vector<ErrorHandler*>		ErrorHandles;
	typedef TimerEngineT<TimerHeap>			TimerEngine;

	class EventPoller
	{
	public:
		EventPoller(int size = 1024 * 10);
		~EventPoller(void);

		virtual bool DoRegisterRead(int fd,int id);
		virtual bool DoRegisterWrite(int fd,int id);
		virtual bool DoRegisterError(int fd,int id);

		virtual bool DoDeRegisterRead(int fd,int id);
		virtual bool DoDeRegisterWrite(int fd,int id);
		virtual bool DoDeRegisterError(int fd,int id);

		bool RegisterRead(int id,int fd,InputHandler* handler);
		bool RegisterWrite(int id,int fd,OutputHandler* handler);
		bool RegisterError(int id,int fd,ErrorHandler* handler);

		bool DeRegisterRead(int id,int fd);
		bool DeRegisterWrite(int id,int fd);
		bool DeRegisterError(int id,int fd);

		virtual bool HandleRead(int id,int fd);
		virtual bool HandleWrite(int id,int fd);
		virtual bool HandleError(int id,int fd);

		virtual bool isRegistered(int id, bool isForRead);
		virtual bool isRegisteredError(int id);

		virtual int Process();

		virtual int ProcessEvents() = 0;

		virtual int  GenId(int fd);
		virtual void RetrieveId(int fd,int id);
		
		TimerEngine& Timer();
		
		static EventPoller* Create();

	protected:
		int					_maxSize;
		std::queue<int>		_idPool;
		IDMap				_idMap;
		ReadHandles			_readHandles;
		WriteHandles		_writeHandles;
		ErrorHandles		_errorHandles;
		TimerEngine			_timerEngine;
	};

}

#endif