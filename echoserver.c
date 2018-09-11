#include <sys/epoll.h>
#include "csapp.h"

#define MAXEVENTS 500

#define CHK(res, eval, msg) if((res = eval) < 0) { fprintf(stderr, "%s: %s\n", msg, strerror(errno)); exit(0); }
#define CHK2(eval, msg) if(eval < 0) { fprintf(stderr, "%s: %s\n", msg, strerror(errno)); exit(0); }

void setnonblock(int fd) {
	int flag;
	CHK(flag, fcntl(fd, F_GETFL, 0), "Get flag error");
	CHK2(fcntl(fd, F_SETFL, flag | O_NONBLOCK), "Set non-blocking error");
}

void echo(int connfd) {
	size_t n;
	char buf[MAXLINE];
	rio_t rio;
	/*
	Rio_readinitb(&rio, connfd);
	while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		printf("server received %d bytes\n", (int)n);
		Rio_writen(connfd, buf, n);
	}
	*/
	recv(connfd, buf, MAXLINE, 0);
	/*
	printf("server received %d bytes\n", (int)n);
	write(connfd, buf, n);
	*/
	Close(connfd);
}

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	int epollfd, listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;
	char client_hostname[MAXLINE], client_port[MAXLINE];
	static struct epoll_event ev, events[MAXEVENTS];
	int epoll_event_num;

	CHK(epollfd, epoll_create(MAXEVENTS), "Epoll create error");
	listenfd = Open_listenfd(argv[1]);
	setnonblock(listenfd);

	ev.data.fd = listenfd;
	ev.events = EPOLLIN | EPOLLET;
    CHK2(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev), "Epoll control error");

	while (1) {
		CHK(epoll_event_num, epoll_wait(epollfd, events, MAXEVENTS, -1), "Epoll wait error");
		int i;
		for (i = 0; i < epoll_event_num; ++i) {
			if (events[i].data.fd == listenfd) {
				clientlen = sizeof(struct sockaddr_storage);
				connfd = Accept(listenfd, (SA *)&clientaddr, (int *)&clientlen);
				setnonblock(connfd);
				/*
				Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
				printf("Connected to (%s, %s)\n", client_hostname, client_port);
				*/
				ev.data.fd = connfd;
				ev.events = EPOLLIN | EPOLLET;
				CHK2(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev), "Epoll control error");
			}
			else {
				echo(events[i].data.fd);
			}
		}
	}
	Close(listenfd);
	Close(epollfd);
	exit(0);
}