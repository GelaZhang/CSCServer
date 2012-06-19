/*
 * main.cpp
 *
 *  Created on: Jun 13, 2012
 *      Author: sundayman
 */

#include "net_service.h"

#include "embassy.h"

#include <stdlib.h>

static int count = 0;


void *test_func(void *arg)
{
        int i=0;
        for(i=0;i<20000;++i){
                __sync_fetch_and_add(&count,1);
        }
        return NULL;
}
int main(int argc, char* argv[]) {

	Embassy embassy;
	NetService svr(5054);
	svr.Init(&embassy);
	svr.Start();
	return 0;
}

