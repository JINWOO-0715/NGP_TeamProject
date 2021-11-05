#pragma once

#include "TCPSocket.h"

class SocketUtil
{
public:
	static bool StaticInit();
	static void StaticShutdown();
	static void ReportError(const char* desc);
	static int GetLastError();
	static TCPSocketPtr CreateTCPSocket();

	static int Select(const vector< TCPSocketPtr >* inReadSet,
		vector< TCPSocketPtr >* outReadSet,
		const vector< TCPSocketPtr >* inWriteSet,
		vector< TCPSocketPtr >* outWriteSet,
		const vector< TCPSocketPtr >* inExceptSet,
		vector< TCPSocketPtr >* outExceptSet);

private:
	inline static fd_set* FillSetFromVector(fd_set& outSet, const vector< TCPSocketPtr >* inSockets, int& ioNaxNfds);
	inline static void FillVectorFromSet(vector< TCPSocketPtr >* outSockets, const vector< TCPSocketPtr >* inSockets, const fd_set& inSet);
};

