#ifndef __SRVMNG_H__
#define __SRVMNG_H__

typedef struct SrvMng SrvMng;

SrvMng* SrvMngCreate();

void SrvMngDestroy(SrvMng** _srvMng);

void SrvMngActivate(SrvMng* _srvMng);

#endif /* __SRVMNG_H__ */
