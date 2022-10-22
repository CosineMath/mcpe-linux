
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

struct bionic_addrinfo {
	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST */
	int	ai_family;	/* PF_xxx */
	int	ai_socktype;	/* SOCK_xxx */
	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	socklen_t ai_addrlen;	/* length of ai_addr */
	char	*ai_canonname;	/* canonical name for hostname */
	struct	sockaddr *ai_addr;	/* binary address */
	struct	bionic_addrinfo *ai_next;	/* next structure in linked list */
};

struct bionic_addrinfo* bionic_copy_addrinfo(struct addrinfo* info) {
    if (info == NULL) {
        return NULL;
    }
    struct bionic_addrinfo* retval = malloc(sizeof(struct bionic_addrinfo));
    retval->ai_flags = info->ai_flags;
    retval->ai_family = info->ai_family;
    retval->ai_socktype = info->ai_socktype;
    retval->ai_protocol = info->ai_protocol;
    retval->ai_addrlen = info->ai_addrlen;
    retval->ai_canonname = info->ai_canonname;
    retval->ai_addr = info->ai_addr;
    retval->ai_next = bionic_copy_addrinfo(info->ai_next);
    return retval;
}

void bionic_freeaddrinfo(struct bionic_addrinfo* info) {
    if (info == NULL) {
        return;
    }
    if (info->ai_next != NULL) {
        bionic_freeaddrinfo(info->ai_next);
    }
    free(info);
}

int bionic_getaddrinfo(const char *node, const char *service,
                       const struct bionic_addrinfo *hints,
                       struct bionic_addrinfo **res) {
    struct addrinfo gnu_hints;
    memset(&gnu_hints, 0, sizeof(gnu_hints));
    gnu_hints.ai_family = hints->ai_family;
    gnu_hints.ai_socktype = hints->ai_socktype;
    gnu_hints.ai_protocol = hints->ai_protocol;
    gnu_hints.ai_flags = hints->ai_flags;
    
    struct addrinfo* gnu_res = NULL;
    int ret = getaddrinfo(node, service, &gnu_hints, &gnu_res);
    *res = bionic_copy_addrinfo(gnu_res);
    freeaddrinfo(gnu_res);
    return ret;
}

#define	bionic_NI_NOFQDN	0x00000001
#define	bionic_NI_NUMERICHOST	0x00000002
#define	bionic_NI_NAMEREQD	0x00000004
#define	bionic_NI_NUMERICSERV	0x00000008
#define	bionic_NI_DGRAM		0x00000010

int bionic_getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                       char *host, socklen_t hostlen,
                       char *serv, socklen_t servlen, int bionic_flags) {
    int flags = 0;
    if (bionic_flags & bionic_NI_NOFQDN) {
        flags |= NI_NOFQDN;
    }
    if (bionic_flags & bionic_NI_NUMERICHOST) {
        flags |= NI_NUMERICHOST;
    }
    if (bionic_flags & bionic_NI_NAMEREQD) {
        flags |= NI_NAMEREQD;
    }
    if (bionic_flags & bionic_NI_NUMERICSERV) {
        flags |= NI_NUMERICSERV;
    }
    if (bionic_flags & bionic_NI_DGRAM) {
        flags |= NI_DGRAM;
    }
    return getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
}
