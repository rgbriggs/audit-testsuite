#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <fcntl.h>

#include <errno.h>
#include <string.h>

int nl_open(void) {
        int fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_AUDIT);
        int saved_errno;
        
        if (fd < 0) {
		//perror("ERROR opening socket");
                saved_errno = errno;
                if (errno == EINVAL || errno == EPROTONOSUPPORT ||
                                errno == EAFNOSUPPORT)
                        printf(
                                "Error - audit support not in kernel");
                else
                        printf(
                                "Error opening audit netlink socket (%s)",
                                strerror(errno));
                errno = saved_errno;
                return fd;
        }       
        if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
                saved_errno = errno;
                close(fd);
                printf(
                        "Error setting audit netlink socket CLOEXEC flag (%s)",
                        strerror(errno));
                errno = saved_errno;
                return -1;
        }       
	//printf(".");
        return fd;
}       

void nl_close(int fd) {
	close(fd);
	//printf(":");
}

#define PORTID_QTY 1020
int main(int argc, char *argv[])
{
	int fd[PORTID_QTY][2];
	int i;

	for (i = 0; i < PORTID_QTY; i++) {
		fd[i][0] = nl_open();
		//fd[i][1] = getsockopt(SOL_NETLINK,);
	}
	while(1) {
		for (i = 0; i < PORTID_QTY; i++) {
			nl_close(fd[i][0]);
			fd[i][0] = nl_open();
			//fd[i][1] = getsockopt(SOL_NETLINK,);
		}
	}
	return 0;
}
