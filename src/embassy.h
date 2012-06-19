/* 
 * filename : embassy.h
 * author   : sundayman
 * date     : Jun 15, 2012
 * brief    :
 */

#ifndef EMBASSY_H_
#define EMBASSY_H_

/**
 * \brief embassy service for protocol analysis layer.
 * \	  if you want to change default action of the embassy ,
 * \		 you can inherit class Embassy and then implement your version
 */

#include "diplomat.h"

class Embassy {

public:
	virtual ~Embassy(){}
	virtual int GarrisonDiplomat(const DiplomatPtr &diplomat);
	virtual int WithdrawDiplomat(const DiplomatPtr &diplomat);
	virtual int RecvSomething(const DiplomatPtr &diplomat);
};


#endif /* EMBASSY_H_ */
