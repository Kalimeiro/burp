#ifndef _DELETE_SERVER_H
#define _DELETE_SERVER_H

extern int delete_backups(struct sdirs *sdirs, const char *cname,
	struct strlist *keep);

extern int do_delete_server(struct asfd *asfd,
	struct sdirs *sdirs, struct cntr *cntr,
	const char *cname, const char *backup);

#endif
