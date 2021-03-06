#ifndef SESSION_H
#define SESSION_H
#include <vector>
#include <map>
#include <queue>

#include "Interface.h"
#include "EventPoller.h"
#include "TcpReader.h"
#include "../ServerApp.h"
#include "../util/MemoryStream.h"



class ServerApp;

namespace Network
{
	class Session:public Network::InputHandler,
		public Network::ErrorHandler,
		public Network::OutputHandler
	{	
	public:
		enum SessionState {Alive,Closed,Error,Invalid};

		struct SendBuffer
		{
			SendBuffer(int size = 1024):_size(size),_roffset(0),_woffset(0),_next(NULL)
			{
				_data = malloc(_size);
			}

			~SendBuffer()
			{
				free(_data);
			}

			void Append(void* data,int size)
			{
				Reserve(size);
				memcpy((char*)_data+_woffset,data,size);
				_woffset += size;
			}

			void* Begin()
			{
				return (void*)((char*)_data+_roffset);
			}

			void SetOffset(int off)
			{
				_roffset += off;
			}

			void Reset()
			{
				_woffset = _roffset = 0;
				_next = NULL;
			}

			int Readable()
			{
				return _woffset - _roffset;
			}

			int Writable()
			{
				return _size - _woffset;
			}

			void Reserve(int size)
			{
				if (_woffset+size <= _size)
					return;
				int nsize = _woffset + _size * 2;
				if (nsize < size)
					nsize = size;
				_data = (char*)realloc(_data,nsize);
				_size = nsize;
			}

			int			_size;
			int			_roffset;
			int			_woffset;
			void*		_data;
			SendBuffer* _next;
		};

		struct SendList
		{
			int _size;
			SendBuffer* _head;
			SendBuffer* _tail;
			SendBuffer* _freelist;

			SendList(int size = 1024)
			{
				_size = size;
				_head = _tail = _freelist = NULL;
			}

			~SendList()
			{
				SendBuffer* cur = NULL;
				while ((cur = _head) != NULL)
				{
					_head = cur->_next;
					delete cur;
				}
				cur = NULL;
				while ((cur = _freelist) != NULL)
				{
					_freelist = cur->_next;
					delete cur;
				}
			}

			bool Empty()
			{
				return _head == NULL;
			}

			SendBuffer* AllocBuffer()
			{
				if (_freelist == NULL)
				{
					SendBuffer* buffer = new SendBuffer(_size);
					_freelist = buffer;
				}
				SendBuffer* cur = _freelist;
				_freelist = cur->_next;
				cur->Reset();
				return cur;
			}

			SendBuffer* Front()
			{
				return _head;
			}

			void RemoveFront()
			{
				assert(_head != NULL);
				SendBuffer* cur = _head;
				_head = _head->_next;
				if (_head == NULL)
					_tail = NULL;
				FreeBuffer(cur);
			}

			void FreeBuffer(SendBuffer* buffer)
			{
				buffer->_next = _freelist;
				_freelist = buffer;
			}

			void Append(void* data,int size)
			{
				int offset = 0;
				if (_tail == NULL)
				{
					assert(_head == NULL);
					_head = _tail = AllocBuffer();
				}
again:
				int left = _tail->Writable();
				if (left >= size)
				{
					_tail->Append(((char*)data) + offset,size);
					return;
				}
				else
				{
					_tail->Append(((char*)data) + offset,left);
					offset += left;
					size -= left;

					SendBuffer* buffer = AllocBuffer();
					_tail->_next = buffer;
					_tail = buffer;
					goto again;
				}
			}
		};
	public:
		Session(Network::EventPoller* poller,int fd,int buffersize = 64 * 1024);
		virtual ~Session();

		virtual int Init() = 0;

		virtual int Fina() = 0;

		virtual int Forward(const char * ptr,int size) = 0;

		virtual int HandleInput();

		virtual int HandleOutput();

		virtual int HandleError();

		virtual int Clean();

		virtual int Close();

		virtual int Send(char* data,int size);

		virtual int Send(MemoryStream* ms);

		virtual void SetId(int id);

		virtual int  GetId();

		virtual void SetFd(int fd);

		virtual int	 GetFd();

		virtual void SetReader(Reader * reader);

		virtual bool IsAlive();

	private:
		virtual int DoSend();

		virtual int TrySend();

	protected:
		Network::EventPoller *	_poller;
		int						_id;
		int						_fd;
		Reader*					_reader;
		SendList				_sendlist;
		SessionState			_state;
	};
}


#endif
