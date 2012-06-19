/* 
 * filename : identity_master.cpp
 * author   : sundayman
 * date     : Jun 18, 2012
 * brief    : 
 */

#include "identity_master.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>

using namespace std;

IdentityMaster::IdentityMaster(const char *key/* = NULL*/):
_index(1)
{
	if ( NULL == key ) {

		_key = "Name";
	}
	else {

		_key = key;
	}
}

bool IdentityMaster::AddIdentity(const char *identity)
{
	if (NULL == identity)
		return false;

	if ( 0 != memcmp( _key.c_str(), identity, _key.size() * sizeof(char)))
		return false;

	char* end_ptr = NULL;
	unsigned long index = strtoul(identity + _key.size(), &end_ptr, 10);
	ostringstream o;
	o << index;
	string str_tmp = o.str();
	size_t len = strlen(identity) - _key.size();
	if (len != str_tmp.size() ) //长度不相等时，可能出现这种情况：2X结果被转换成2
		return false;

	AutoMutex mutex(_mutex);
	if ( index <= 0 )
		return false;
	if (index > _index) {
		_map_using_identity[index] = 0; //加入已使用零散名称列表
	} else if (index == _index) {
		_index++;
		for (;;) {  //
			MapInt2Char::iterator itor = _map_using_identity.find(_index);
			if ( itor != _map_using_identity.end() ) {
				_index++;
				_map_using_identity.erase(itor);
			} else {
				break;
			}
		}

	} else {

		_map_idle_identity.erase(index);
	}


	return true;
}

bool IdentityMaster::GetIdentity(string& identity)
{
	AutoMutex mutex(_mutex);
	unsigned long index = _index;
	MapInt2Char::iterator itor = _map_idle_identity.begin();
	if ( itor != _map_idle_identity.end() ) {

		index = itor->first;
		_map_idle_identity.erase(itor);
	}
	else {

		_index++;
		//合并离散的键
		for ( ;; ) {

			MapInt2Char::iterator itor = _map_using_identity.find(_index);
			if ( itor != _map_using_identity.end() ) {

				_map_using_identity.erase(itor);
				_index ++;
			} else
				break;
		}

	}


	ostringstream o;
	o<<_key.c_str()<<index;
	identity = o.str();

	return true;
}

bool IdentityMaster::ReleaseIdentity(const string& identity)
{
	if ( 0 != memcmp( _key.c_str(), identity.c_str(), _key.size() * sizeof(char)))
		return false; //no belong to the master

	char* end_ptr = NULL;
	unsigned long index = strtoul(identity.c_str() + _key.size(), &end_ptr, 10);

	ostringstream o;
	o << index;
	string tmp = o.str();
	size_t len = identity.size() - _key.size();
	if ( len != tmp.size() )	//长度不相等时，可能出现这种情况：2X结果被转换成2
		return false;
	if ( index <= 0 )
		return false; //no belong to the master

	AutoMutex mutex(_mutex);
	if (index > _index) {

		MapInt2Char::iterator itor = _map_using_identity.find(index);
		if (itor != _map_using_identity.end()) {

			_map_using_identity.erase(itor);
		}
	} else if (index < _index) {

		_map_idle_identity[index] = 0;
		for (;;) {
			MapInt2Char::iterator itor = _map_idle_identity.find(_index - 1);
			if (itor != _map_idle_identity.end()) {
				_map_idle_identity.erase(itor);
				_index--;
			} else {
				break;
			}

		}

	}
	return true;
}


