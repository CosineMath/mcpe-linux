
#include <android/looper.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <stddef.h>

struct ALooper {
    int epoll_fd;
};

static __thread ALooper* mLooper = NULL;

ALooper* ALooper_prepare(int opts) {
    if (mLooper == NULL) {
        mLooper = malloc(sizeof(ALooper));
        mLooper->epoll_fd = epoll_create(1);
    }
    return mLooper;
}

typedef struct {
    int fd;
    int ident;
    int events;
    ALooper_callbackFunc callback;
    void* data;
} ALooperData;

int ALooper_addFd(ALooper* looper, int fd, int ident, int events,
        ALooper_callbackFunc callback, void* data) {
    if (ident < 0 && callback == NULL) {
        return -1;
    }
    struct epoll_event ev;
    ev.events = EPOLLERR | EPOLLHUP;
    if (events & ALOOPER_EVENT_INPUT) {
        ev.events |= EPOLLIN;
    }
    if (events & ALOOPER_EVENT_OUTPUT) {
        ev.events |= EPOLLOUT;
    }
    ALooperData* priv = malloc(sizeof(ALooperData));
    priv->fd = fd;
    priv->ident = ident;
    priv->events = events;
    priv->callback = callback;
    priv->data = data;
    ev.data.ptr = priv;

    int ret = epoll_ctl(looper->epoll_fd, EPOLL_CTL_ADD, fd, &ev); 
    return (ret == 0) ? 1 : -1;
}

int ALooper_pollAll(int timeoutMillis, int* outFd, int* outEvents, void** outData) {
    if (mLooper == NULL) {
        return ALOOPER_POLL_ERROR;
    }
    struct epoll_event ev;
    int ret = epoll_wait(mLooper->epoll_fd, &ev, 1, timeoutMillis);
    if (ret == 1) {
        ALooperData* priv = ev.data.ptr;
        if (priv->callback) {
            priv->callback(priv->fd, priv->events, priv->data);
            return ALOOPER_POLL_CALLBACK;
        }
        else {
            if (outFd != NULL) {
                *outFd = priv->fd;
            }
            if (outEvents != NULL) {
                int events = 0;
                if (ev.events & EPOLLIN) {
                    events |= ALOOPER_EVENT_INPUT;
                }
                if (ev.events & EPOLLOUT) {
                    events |= ALOOPER_EVENT_OUTPUT;
                }
                *outEvents = events;
            }
            if (outData != NULL) {
                *outData = priv->data;
            }
            return priv->ident;
        }
    }
    else if (ret == 0) {
        return ALOOPER_POLL_TIMEOUT;
    }
    return ALOOPER_POLL_ERROR;
}




