#ifndef NETCORE_ERRORCODE_H_
#define NETCORE_ERRORCODE_H_
namespace netcore {
	const int OK = 0;
	const int ERROR_ALLOC_MEMORY = -1;
	const int ERROR_WRITE_FAILURE = -2;
	const int ERROR_NOT_READABLE = -3;
	const int ERROR_NOT_SEND_ALL = -4;
	const int ERROR_EMPTY_BUFFER = -5;
	const int ERROR_NULL_POINTER = -6;
	const int ERROR_LOCAL_PORT_USED = -7;
	const int ERROR_SESSION_NOT_EXIST = -8;
	const int ERROR_SESSION_NOT_CREATE = -9;
	const int ERROR_SYSTEM_TIMEOUT = -10;
	const int ERROR_CONNECT_EXCEPTION = -11;
	const int ERROR_NOT_USE = -12;
	const int ERROR_PARAM = -13;
}
#endif // NETCORE_ERRORCODE_H_
