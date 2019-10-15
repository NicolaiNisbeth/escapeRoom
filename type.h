//
// Created by nicolainisbeth on 15/10/2019.
//

#ifndef ESCAPEROOM_TYPE_H
#define ESCAPEROOM_TYPE_H

struct Group {
    pthread_t id;
    char name;
    int inEscapeRoom;
};

struct Instructor {
    pthread_t id;
    char *name;
    int sleepTime;
};
#endif //ESCAPEROOM_TYPE_H
