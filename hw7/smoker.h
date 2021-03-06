/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _SMOKER_H_RPCGEN
#define _SMOKER_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif


struct smoker_info {
	int material;
	int amount;
	int id;
	int done;
};
typedef struct smoker_info smoker_info;

struct smoker_id {
	int id;
	int logging;
};
typedef struct smoker_id smoker_id;

#define SMOKER_PROG 0x1059860
#define SMOKER_VERS 1

#if defined(__STDC__) || defined(__cplusplus)
#define SMOKER_START 1
extern  int * smoker_start_1(struct smoker_id *, CLIENT *);
extern  int * smoker_start_1_svc(struct smoker_id *, struct svc_req *);
#define SMOKER_PROC 2
extern  int * smoker_proc_1(struct smoker_info *, CLIENT *);
extern  int * smoker_proc_1_svc(struct smoker_info *, struct svc_req *);
#define SMOKER_EXIT 3
extern  void * smoker_exit_1(struct smoker_id *, CLIENT *);
extern  void * smoker_exit_1_svc(struct smoker_id *, struct svc_req *);
extern int smoker_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define SMOKER_START 1
extern  int * smoker_start_1();
extern  int * smoker_start_1_svc();
#define SMOKER_PROC 2
extern  int * smoker_proc_1();
extern  int * smoker_proc_1_svc();
#define SMOKER_EXIT 3
extern  void * smoker_exit_1();
extern  void * smoker_exit_1_svc();
extern int smoker_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_smoker_info (XDR *, smoker_info*);
extern  bool_t xdr_smoker_id (XDR *, smoker_id*);

#else /* K&R C */
extern bool_t xdr_smoker_info ();
extern bool_t xdr_smoker_id ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_SMOKER_H_RPCGEN */
