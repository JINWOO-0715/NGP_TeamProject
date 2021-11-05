#include "PongPCH.h"
#include "SocketUtil.h"

bool SocketUtil::StaticInit()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		ReportError("SocketUtil::StaticInit");
		return false;
	}
	else
	{
		return true;
	}
}

void SocketUtil::StaticShutdown()
{
	WSACleanup();
}

void SocketUtil::ReportError(const char* desc)
{
	LPVOID lpMsgBuf;
	DWORD errorNum = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errorNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);


	LOG("Error {0}: {1} - {2}", desc, errorNum, lpMsgBuf);
}

int SocketUtil::GetLastError()
{
	return WSAGetLastError();
}

TCPSocketPtr SocketUtil::CreateTCPSocket()
{
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == INVALID_SOCKET)
	{
		ReportError("SocketUtil::CreateTCPSocket");
		return nullptr;
	}
	else
	{
		return TCPSocketPtr(new TCPSocket(sock));
	}
}

int SocketUtil::Select(const vector< TCPSocketPtr >* inReadSet, vector< TCPSocketPtr >* outReadSet, const vector< TCPSocketPtr >* inWriteSet, vector< TCPSocketPtr >* outWriteSet, const vector< TCPSocketPtr >* inExceptSet, vector< TCPSocketPtr >* outExceptSet)
{
	fd_set read, write, except;

	int nfds = 0;

	fd_set* readPtr = FillSetFromVector(read, inReadSet, nfds);
	fd_set* writePtr = FillSetFromVector(read, inWriteSet, nfds);
	fd_set* exceptPtr = FillSetFromVector(read, inExceptSet, nfds);

	int toRet = select(nfds + 1, readPtr, writePtr, exceptPtr, nullptr);

	if (toRet > 0)
	{
		FillVectorFromSet(outReadSet, inReadSet, read);
		FillVectorFromSet(outWriteSet, inWriteSet, write);
		FillVectorFromSet(outExceptSet, inExceptSet, except);
	}
	return toRet;
}

fd_set* SocketUtil::FillSetFromVector(fd_set& outSet, const vector< TCPSocketPtr >* inSockets, int& ioNaxNfds)
{
	if (inSockets)
	{
		FD_ZERO(&outSet);

		for (const TCPSocketPtr& socket : *inSockets)
		{
			FD_SET(socket->mSocket, &outSet);
		}
		return &outSet;
	}
	else
	{
		return nullptr;
	}
}

void SocketUtil::FillVectorFromSet(vector< TCPSocketPtr >* outSockets, const vector< TCPSocketPtr >* inSockets, const fd_set& inSet)
{
	if (inSockets && outSockets)
	{
		outSockets->clear();
		for (const TCPSocketPtr& socket : *inSockets)
		{
			if (FD_ISSET(socket->mSocket, &inSet))
			{
				outSockets->push_back(socket);
			}
		}
	}
}
