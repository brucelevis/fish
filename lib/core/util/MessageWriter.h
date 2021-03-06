#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H
#include "../Typedef.h"

#include <string>
#include <vector>

namespace MessageHelper
{
	class MessageWriter
	{
	public:
		MessageWriter(int size = 64);
		~MessageWriter();

		MessageWriter& operator<<(bool value);
		MessageWriter& operator<<(uint8 value);
		MessageWriter& operator<<(uint16 value);
		MessageWriter& operator<<(uint32 value);
		MessageWriter& operator<<(uint64 value);
		MessageWriter& operator<<(int8 value);
		MessageWriter& operator<<(int16 value);
		MessageWriter& operator<<(int32 value);
		MessageWriter& operator<<(int64 value);
		MessageWriter& operator<<(float value);
		MessageWriter& operator<<(double value);
		MessageWriter& operator<<(const std::string& str);
		MessageWriter& operator<<(const char *str);

		MessageWriter& AppendString(const char* str,int size);
		
		char* Data();
		void Reset();
		int Length();

		void reserve(int cnt);
		void append(uint8* type,uint8* val,int size);
		void append(char* str,int size);

		char* _data;
		int _offset;
		int _size;
	};
}


#endif