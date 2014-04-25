#ifndef RUN_SCRIPT_H
#define RUN_SCRIPT_H

extern int run_script(struct async *as,
	const char **args, struct strlist *userargs,
	struct conf *conf, int do_wait, int do_logp, int do_logw);

extern int run_script_to_buf(struct async *as,
	const char **args, struct strlist *userargs, struct conf *conf,
        int do_wait, int do_logp, int do_logw, char **logbuf);

#endif
