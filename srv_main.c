#include "srv_mng.h"
#include <stdio.h>
int main(void)
{
	SrvMng* srvMng = SrvMngCreate();
	SrvMngActivate(srvMng);
	SrvMngDestroy(&srvMng);
	return 0;
}

