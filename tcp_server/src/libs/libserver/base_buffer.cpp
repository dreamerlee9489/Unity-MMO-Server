#include "base_buffer.h"
#include "log4_help.h"

#include <iostream>
#include <cstring>

unsigned Buffer::GetEmptySize()
{
	return _bufferSize - _endIndex;
}

void Buffer::ReAllocBuffer(const unsigned int dataLength)
{
	// ���������������󻺳�ֵ����������
	if (_bufferSize >= MAX_SIZE) {
		std::cout << "Buffer::Realloc except!! Max size:" << _bufferSize << std::endl;
	}

	char* tempBuffer = new char[_bufferSize + ADDITIONAL_SIZE];
	unsigned int _newEndIndex;
	if (_beginIndex < _endIndex)
	{
		::memcpy(tempBuffer, _buffer + _beginIndex, _endIndex - _beginIndex);
		_newEndIndex = _endIndex - _beginIndex;
	}
	else
	{
		if (_beginIndex == _endIndex && dataLength <= 0)
		{
			_newEndIndex = 0;
		}
		else 
		{
			// 1.��COPYβ��
			::memcpy(tempBuffer, _buffer + _beginIndex, _bufferSize - _beginIndex);
			_newEndIndex = _bufferSize - _beginIndex;

			// 2.��COPYͷ��
			if (_endIndex > 0)
			{
				::memcpy(tempBuffer + _newEndIndex, _buffer, _endIndex);
				_newEndIndex += _endIndex;
			}
		}
	}

	// �޸�����
	_bufferSize += ADDITIONAL_SIZE;

	delete[] _buffer;
	_buffer = tempBuffer;

	_beginIndex = 0;
	_endIndex = _newEndIndex;

#if TestNetwork
	LOG_WARN("Buffer::Realloc. _bufferSize:" << _bufferSize );
#endif
}

