//
// Created by nicolainisbeth on 17/10/2019.
//

#ifndef ESCAPEROOM_STRINGSTORE_H
#define ESCAPEROOM_STRINGSTORE_H

char *NAMES[] = {"Alicia", "Jessica", "Rachel", "Linda"};
char *INSTRUCTOR_MSG = "Instructor %s welcomes %c to the escape room\t\t\t\t\t\t\t\t\t\t\t\tTaken chairs = %d/%d\n";
char *WELCOME_MSG = "Group %c takes a chair in waiting room\t\t\t\t\t\t\t\t\t\t\t\t\t\tTaken chairs = %d/%d\n";
char *ANSWER_MSG = "Group %c changes global mystery %d -> %d\n";
char *NO_FREE_CHAIR_MSG = "Group %c cannot find a free chair and leaves!\n";
char *WINNER = "winner";
char *LOSER = "loser";
char *CONGRATULATIONS_MSG = "Congratulations ";
char *UNLUCKY_MSG = "Unlucky ";
char *GROUP_NAME_MSG = "group %c ";
char *MYSTERY_SOLVED_MSG = "The mystery is solved!\n";
char *MYSTERY_FAILED_MSG = "The mystery is NOT solved!\n";
char *FULL_QUEUE_MSG = "All chairs are taken!\n";
char *EMPTY_QUEUE_MSG = "No chairs are taken!";

#endif //ESCAPEROOM_STRINGSTORE_H