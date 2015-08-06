#ifndef _CLIENT_VSS_H
#define _CLIENT_VSS_H

#if defined(WIN32_VSS)
extern int win32_start_vss(struct conf **confs);
extern int win32_stop_vss(void);
extern int get_vss(BFILE *bfd, struct sbuf *sb,
	char **vssdata, size_t *vlen);
extern int set_vss(BFILE *bfd, const char *vssdata, size_t vlen);
#endif // WIN32_VSS

#if defined(HAVE_WIN32)
extern int win32_enable_backup_privileges();
#endif  /* HAVE_WIN32 */

#endif
