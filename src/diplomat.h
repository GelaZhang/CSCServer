/* 
 * filename : diplomat.h
 * author   : sundayman
 * date     : Jun 13, 2012
 * brief    : 
 */

#ifndef DIPLOMAT_H_
#define DIPLOMAT_H_


class Diplomat {

private:
	friend class DiplomatMaster;
	/**
	 * brief forbidden new object except DiplomatMaster
	 */
	Diplomat(struct bufferevent *bev);
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
};


#endif /* DIPLOMAT_H_ */
