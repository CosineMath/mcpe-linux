
#include <android/input.h>

#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

struct AInputEvent {
    // for InputQueue
    AInputEvent* next;
    int32_t device_id;
    int32_t type;
    int32_t source;
    union {
        struct {
            int32_t action;
            int32_t code;
            int32_t state;
            int32_t repeat;
        } key;
        struct {
            int32_t action;
            float x;
            float y;
        } motion;
    };
};

// Implementation
AInputEvent* AInputEvent_create(int32_t type, int32_t source, ...) {
    AInputEvent* event = malloc(sizeof(AInputEvent));
    event->next = NULL;
    event->device_id = 0;
    event->type = type;
    event->source = source;
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        va_list ap;
        va_start(ap, source);
        event->motion.action = va_arg(ap, int32_t);
        event->motion.x = va_arg(ap, double);
        event->motion.y = va_arg(ap, double);
        va_end(ap);
    }
    else if (type == AINPUT_EVENT_TYPE_KEY) {
        va_list ap;
        va_start(ap, source);
        event->key.action = va_arg(ap, int32_t);
        event->key.code = va_arg(ap, int32_t);
        event->key.state = va_arg(ap, int32_t);
        event->key.repeat = va_arg(ap, int32_t);
        va_end(ap);
    }
    return event;
}

// AInputEvent
int32_t AInputEvent_getDeviceId(const AInputEvent* event) {
    return event->device_id;
}

int32_t AInputEvent_getType(const AInputEvent* event) {
    return event->type;
}

int32_t AInputEvent_getSource(const AInputEvent* event) {
    return event->type;
}

// AKeyEvent
int32_t AKeyEvent_getAction(const AInputEvent* key_event) {
    return key_event->key.action;
}

int32_t AKeyEvent_getKeyCode(const AInputEvent* key_event) {
    return key_event->key.code;
}

int32_t AKeyEvent_getMetaState(const AInputEvent* key_event) {
    return key_event->key.state;
}

int32_t AKeyEvent_getRepeatCount(const AInputEvent* key_event) {
    return key_event->key.repeat;
}

// AMotionEvent
int32_t AMotionEvent_getAction(const AInputEvent* motion_event) {
    return motion_event->motion.action;
}

float AMotionEvent_getAxisValue(const AInputEvent* motion_event,
        int32_t axis, size_t pointer_index) {
    return 0.0f;
}

float AMotionEvent_getRawX(const AInputEvent* motion_event, size_t pointer_index) {
    return motion_event->motion.x;
}

float AMotionEvent_getRawY(const AInputEvent* motion_event, size_t pointer_index) {
    return motion_event->motion.y;
}

float AMotionEvent_getX(const AInputEvent* motion_event, size_t pointer_index) {
    return motion_event->motion.x;
}

float AMotionEvent_getY(const AInputEvent* motion_event, size_t pointer_index) {
    return motion_event->motion.y;
}

size_t AMotionEvent_getPointerCount(const AInputEvent* motion_event) {
    return 1;
}

int32_t AMotionEvent_getPointerId(const AInputEvent* motion_event, size_t pointer_index) {
    return 0;
}

// AInputQueue
struct AInputQueue {
    int count;
    AInputEvent* head;
    AInputEvent* tail;
    pthread_mutex_t mutex;
    int pipe_fd[2];
};

// Implementation
AInputQueue* AInputQueue_create() {
    AInputQueue* queue = malloc(sizeof(AInputQueue));
    pthread_mutex_init(&queue->mutex, NULL);
    queue->count = 0;
    queue->head = NULL;
    queue->tail = NULL;
    pipe(queue->pipe_fd);
    return queue;
}

// Implementation
void AInputQueue_pushEvent(AInputQueue* queue, AInputEvent* e) {
    pthread_mutex_lock(&queue->mutex);
    e->next = NULL;
    if (queue->count > 0) {
        queue->tail->next = e;
        queue->tail = e;
    }
    else { // count == 0
        queue->head = e;
        queue->tail = e;
    }
    queue->count++;
    write(queue->pipe_fd[1], "E", 1);
    pthread_mutex_unlock(&queue->mutex);
}

int32_t AInputQueue_getEvent(AInputQueue* queue, AInputEvent** outEvent) {
    pthread_mutex_lock(&queue->mutex);
    char c;
    int ret = -1;
    AInputEvent* e = NULL;
    if (queue->count > 0) {
        if (read(queue->pipe_fd[0], &c, 1) > 0) {
            e = queue->head;
            queue->head = e->next;
            queue->count--;
            ret = 0;
        }
    }
    *outEvent = e;
    pthread_mutex_unlock(&queue->mutex);
    return ret;
}

void AInputQueue_finishEvent(AInputQueue* queue, AInputEvent* event, int handled) {
    free(event);
}

int32_t AInputQueue_preDispatchEvent(AInputQueue* queue, AInputEvent* event) {
    return 0;
}

void AInputQueue_attachLooper(AInputQueue* queue, ALooper* looper,
        int ident, ALooper_callbackFunc callback, void* data) {
    pthread_mutex_lock(&queue->mutex);
    ALooper_addFd(looper, queue->pipe_fd[0], ident, ALOOPER_EVENT_INPUT, callback, data);
    pthread_mutex_unlock(&queue->mutex);
}

void AInputQueue_detachLooper(AInputQueue* queue) {
    fprintf(stderr, "AInputQueue_detachLooper() is called!\n");
}

