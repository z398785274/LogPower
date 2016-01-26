#ifndef ERR_NO_H
#define ERR_NO_H

enum ErrorValue
{
	kBorderCheckFailed = -20,
	kUnknown = -19,
	kReadFailedOrPeerClosed = -18,
	kSerailizeFailed = -17,
	kQueueIsFull = -16,
	kDelFromEpollFailed = -15,
	kInitThreadRelatedFailed = -14,
	kAddToEpollFailed = -13,
	kCreateEpollFdFailed = -12,
	kListenFailed = -11,
	kBindFailed = -10,
	kCreateSocketFailed = -9,
	kSendDataToServerFailed = -7,
	kNotConnctedToServer = -6,
	kConnectToServerFailed = -5,
	kSetNonBlockFailed = -4,
	kRunSystemCmdFailed= -3,
	kOpenFileFailed = -2,
	kInvalidParams = -1,
	kOk = 0,
	kReadEagain = 1,

};

enum ServerType
{
	AsClient = 0,
	AsServer = 1
};

#endif