/* 
 * filename : command_distributor.h
 * author   : sundayman66@gmail.com
 * date     : 2012-7-11
 * brief    : 解析协议头分发命令
 */

#ifndef COMMAND_DISTRIBUTOR_H_
#define COMMAND_DISTRIBUTOR_H_

#include <map>
#include <string>

#include "diplomat.h"
#include "command/command.h"
#include "embassy.h"

namespace Net {


#ifndef PATH_MAX

#define PATH_MAX 255

#endif
/*
 * brief 协议头结构
 */
struct ProtocolHeader {

    static const char kProtocolStart[];
    static const char kProtocolVersion[];
    static const char kProtocolPubkey[];
    static const char kProtocolSize[];
    static const char kProtocolCmdId[];
    static const char kProtocolSeparator[];
    static const char kProtocolSeparator2[];

    char protocol[50];
    char version[32];
    char pubkey[32 + 1];
    char commandId[128 + 1];
    int size;
    ProtocolHeader();
    std::string BuildHeader(int size, const char *ack_id);
};

class CommandDistributor  : public Embassy{

	static const int kMaxLengthOfHeader;
	static const int kMaxLengthOfMethod;
	static const int kMaxBuffer;
	class DiplomatMemo: public Memo {

	public:
		DiplomatMemo() {

			_header_got = false;
			_header_length = 0;
			_buffer_length = 2048;
			_buffer = new unsigned char[_buffer_length + 1];
			_buffer_offset = 0;
		}

		~DiplomatMemo() {
			if ( _buffer ) {
				delete []_buffer;
				_buffer = NULL;
			}
		}
		/**
		*	\brief 检查存储容量是否充足，不能满足length字节空间，则自动进行扩容
		*/
		void PrepareCapacity(size_t length) {
			if ( _buffer_offset + length >= _buffer_length && _buffer_length < kMaxBuffer ) {
				size_t size = _buffer_offset + length + 1024;
				size = size > kMaxBuffer ? kMaxBuffer : size;
				unsigned char* tmp = new unsigned char[size + 1];
				if ( !tmp )
					return;
				memcpy(tmp, _buffer, _buffer_offset);
				_buffer_length = size;
				delete []_buffer;
				_buffer = tmp;

			}
		}
		void Drain(size_t size) {

			if ( 0 == size || size > _buffer_offset ) {
				_buffer_offset = 0;
			} else {
				_buffer_offset -= size;
				memcpy(_buffer, _buffer + size, _buffer_offset);
			}
		}
		ProtocolHeader _header;
		int _header_length;
		bool _header_got;
		unsigned char* _buffer;
		size_t _buffer_length;
		size_t _buffer_offset;
	};
	typedef Utility::SmartPtr<DiplomatMemo> DiplomatMemoPtr;

	typedef std::map<std::string, CommandPtr> CmdMap;
public:

	virtual int GarrisonDiplomat(const DiplomatPtr &diplomat);
	virtual int WithdrawDiplomat(const DiplomatPtr &diplomat);
	/**
	 * \brief  进行命令分发。
	 * \return 返回成功分发的命令数。
	 */
	virtual int RecvSomething(const DiplomatPtr &diplomat);

	static bool RegisterCommand(const CommandPtr &command);
private:

	/**
	 *  \brief 从数据中提取协议头
	 *  \return 成功返回协议头占用的字节数。协议头无效返回-1，
	 *  		协议头还没完整返回0, 否则返回协议头长度
	 */
	int ParseProtocolHeader(const char* data, DiplomatMemoPtr &memo);

	int GetProtocolHeader(
			const DiplomatPtr &diplomat, char *buffer, int &cursor, int length);

	const char *GetProtocolKeyword(
			const char *data, const char *keyword_start,
			        char *keyword, int keyword_len);

	int ParseCommand(const DiplomatPtr &diplomat, char *content, char *version);

private:

	static CmdMap s_cmds;

};

};
#endif /* COMMAND_DISTRIBUTOR_H_ */


