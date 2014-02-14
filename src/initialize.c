int xseg_posix_init(void);
int xseg_pthread_init(void);
int xseg_posixfd_init(void);

int __xseg_preinit(void)
{
	int r;
	if ((r = xseg_posix_init()))
		goto out;
	if ((r = xseg_pthread_init()))
		goto out;
	if ((r = xseg_posixfd_init()))
		goto out;
out:
	return r;
}
