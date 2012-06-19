/* 
 * filename : diplomat.h
 * author   : sundayman
 * date     : Jun 13, 2012
 * brief    : 
 */

#ifndef DIPLOMAT_H_
#define DIPLOMAT_H_

#include <string>

#include "util/ref_counter.h"

class Diplomat : public Utility::RefCounter {

public:
	/**
	 * \brief write data to socket.
	 * \return kOK when success, otherwise fail
	 */
	int Speek(const char* content, int len);

	inline const std::string &Id() {
		return _id;
	}
private:
	friend class DiplomatMaster;
	/**
	 * brief forbidden new object except DiplomatMaster
	 */
	Diplomat(struct bufferevent *bev, const std::string &id);
	/**
	 * brief forbidden delete object except DiplomatMaster
	 */
	~Diplomat();
	/**
	 * brief forbidden copy object
	 */
	Diplomat& operator = (const Diplomat &);
	/**
	 * brief forbidden copy object
	 */
	Diplomat(const Diplomat&);

private:
	struct bufferevent *_bev;
	std::string _id;
};

typedef Utility::SmartPtr<Diplomat> DiplomatPtr;


#endif /* DIPLOMAT_H_ */
