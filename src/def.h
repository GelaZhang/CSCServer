/*
 * \filename: def.h
 * \brief	:
 * \author	: sundayman
 * \date	: Jun 11, 2012
 */

#ifndef DEF_H_
#define DEF_H_

#ifndef BYTE
#define BYTE unsigned char
#endif

enum ErrorCode {
	kOK = 0,  // everything is good
	kError,   //  unknow error
	kLibEventErr  //something wrong with libevent
};


#endif /* DEF_H_ */
