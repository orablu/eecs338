/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "smoker.h"

bool_t
xdr_smoker_info (XDR *xdrs, smoker_info *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->material))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->amount))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->id))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->done))
				 return FALSE;
		} else {
			IXDR_PUT_LONG(buf, objp->material);
			IXDR_PUT_LONG(buf, objp->amount);
			IXDR_PUT_LONG(buf, objp->id);
			IXDR_PUT_LONG(buf, objp->done);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->material))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->amount))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->id))
				 return FALSE;
			 if (!xdr_int (xdrs, &objp->done))
				 return FALSE;
		} else {
			objp->material = IXDR_GET_LONG(buf);
			objp->amount = IXDR_GET_LONG(buf);
			objp->id = IXDR_GET_LONG(buf);
			objp->done = IXDR_GET_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->material))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->amount))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->done))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_smoker_id (XDR *xdrs, smoker_id *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->id))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->logging))
		 return FALSE;
	return TRUE;
}
