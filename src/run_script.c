#include "include.h"

#ifndef HAVE_WIN32

static int log_script_output(struct asfd *asfd, FILE **fp, struct conf *conf,
	int do_logp, int do_logw, char **logbuf)
{
	char buf[256]="";
	if(!fp || !*fp) return 0;
	if(fgets(buf, sizeof(buf), *fp))
	{
		// logc does not print a prefix
		if(do_logp) logp("%s", buf);
		else logc("%s", buf);
		if(logbuf && astrcat(logbuf, buf, __func__)) return -1;
		if(do_logw) logw(asfd, conf, "%s", buf);
	}
	if(feof(*fp)) close_fp(fp);
	return 0;
}

static int got_sigchld=0;
static int run_script_status=-1;

static void run_script_sigchld_handler(int sig)
{
	//printf("in run_script_sigchld_handler\n");
	got_sigchld=1;
	run_script_status=-1;
	waitpid(-1, &run_script_status, 0);
}

static int run_script_select(struct asfd *asfd, FILE **sout, FILE **serr,
	struct conf *conf, int do_logp, int do_logw, char **logbuf)
{
	int mfd=-1;
	fd_set fsr;
	struct timeval tval;
	int soutfd=fileno(*sout);
	int serrfd=fileno(*serr);
	setlinebuf(*sout);
	setlinebuf(*serr);
	set_non_blocking(soutfd);
	set_non_blocking(serrfd);

	while(1)
	{
		mfd=-1;
		FD_ZERO(&fsr);
		if(*sout) add_fd_to_sets(soutfd, &fsr, NULL, NULL, &mfd);
		if(*serr) add_fd_to_sets(serrfd, &fsr, NULL, NULL, &mfd);
		tval.tv_sec=1;
		tval.tv_usec=0;
		if(select(mfd+1, &fsr, NULL, NULL, &tval)<0)
		{
			if(errno!=EAGAIN && errno!=EINTR)
			{
				logp("%s error: %s\n", __func__,
					strerror(errno));
				return -1;
			}
		}
		if(FD_ISSET(soutfd, &fsr))
		{
			if(log_script_output(asfd, sout, NULL,
				do_logp, do_logw, logbuf)) return -1;
		}
		if(FD_ISSET(serrfd, &fsr))
		{
			if(log_script_output(asfd, serr, conf,
				do_logp, do_logw, logbuf)) return -1;
		}

		if(!*sout && !*serr && got_sigchld)
		{
			//fclose(*sout); *sout=NULL;
			//fclose(*serr); *serr=NULL;
			got_sigchld=0;
			return 0;
		}
	}

	// Never get here.
	return -1;
}

#endif

int run_script_to_buf(struct asfd *asfd,
	const char **args, struct strlist *userargs, struct conf *conf,
	int do_wait, int do_logp, int do_logw, char **logbuf)
{
	int a=0;
	int l=0;
	pid_t p;
	FILE *serr=NULL;
	FILE *sout=NULL;
	char *cmd[64]={ NULL };
	struct strlist *sl;
#ifndef HAVE_WIN32
	int s=0;
#endif
	if(!args || !args[0]) return 0;

	for(a=0; args[a]; a++) cmd[l++]=(char *)args[a];
	for(sl=userargs; sl; sl=sl->next) cmd[l++]=sl->path;
	cmd[l++]=NULL;

#ifndef HAVE_WIN32
	setup_signal(SIGCHLD, run_script_sigchld_handler);
#endif

	fflush(stdout); fflush(stderr);
	if(do_wait)
	{
		if((p=forkchild(NULL,
			&sout, &serr, cmd[0], cmd))==-1) return -1;
	}
	else
	{
		if((p=forkchild_no_wait(NULL,
			&sout, &serr, cmd[0], cmd))==-1) return -1;
		return 0;
	}
#ifdef HAVE_WIN32
	// My windows forkchild currently just executes, then returns.
	return 0;
#else
	s=run_script_select(asfd, &sout, &serr, conf, do_logp, do_logw, logbuf);

	// Set SIGCHLD back to default.
	setup_signal(SIGCHLD, SIG_DFL);

	if(s) return -1;

	if(WIFEXITED(run_script_status))
	{
		int ret=WEXITSTATUS(run_script_status);
		logp("%s returned: %d\n", cmd[0], ret);
		if(do_logw && conf && ret)
			logw(asfd, conf, "%s returned: %d\n", cmd[0], ret);
		return ret;
	}
	else if(WIFSIGNALED(run_script_status))
	{
		logp("%s terminated on signal %d\n",
			cmd[0], WTERMSIG(run_script_status));
		if(do_logw && conf)
			logw(asfd, conf, "%s terminated on signal %d\n",
				cmd[0], WTERMSIG(run_script_status));
	}
	else
	{
		logp("Strange return when trying to run %s\n", cmd[0]);
		if(do_logw && conf) logw(asfd, conf,
			"Strange return when trying to run %s\n",
			cmd[0]);
	}
	return -1;
#endif
}

int run_script(struct asfd *asfd, const char **args, struct strlist *userargs,
	struct conf *conf, int do_wait, int do_logp, int do_logw)
{
	return run_script_to_buf(asfd, args, userargs, conf, do_wait,
		do_logp, do_logw, NULL /* do not save output to buffer */);
}
