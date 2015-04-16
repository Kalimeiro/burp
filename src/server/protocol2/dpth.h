#ifndef __DPTH_PROTOCOL2_H
#define __DPTH_PROTOCOL2_H

#include "../dpth.h"

extern int dpth_init(struct dpth *dpth, const char *base_path,
	int max_storage_subdirs);

extern int dpth_incr_sig(struct dpth *dpth);
extern char *dpth_mk(struct dpth *dpth);
extern char *dpth_get_save_path(struct dpth *dpth);

extern int dpth_fwrite(struct dpth *dpth,
	struct iobuf *iobuf, struct blk *blk);

#endif
