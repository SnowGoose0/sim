#ifndef MESSAGES_H
#define MESSAGES_H

#define ARGS "n:e:h"

#define HELP_DESCR "SIM - SIMple Text Editor (%s)\n\
\n\
Usage: sim [arguments] [file ..]\n\
     : sim [arguments]\n\
\n\
Note: Only one argument can be used at a time\
\n\
Arguments:\n\
    -n                          new file\n\
    -e                          edit existing file\n\
    -r                          read existing file\n\
    -l                          start at line number\n\
    -h or --help                help message\n"

#define INVALID_FLAG_DESCR "Invalid or conflicting options\
\n\
Please use -h flag to see available options\n"

#define MODIFIED_DESCR "Save the buffer? (y or n): "

#define SPLASH "\n_____/\\\\\\\\\\\\\\\\\\\\\\____/\\\\\\\\\\\\\\\\\\\\\\__/\\\\\\\\____________/\\\\\\\\_        \n ___/\\\\\\/////////\\\\\\_\\/////\\\\\\///__\\/\\\\\\\\\\\\________/\\\\\\\\\\\\_       \n  __\\//\\\\\\______\\///______\\/\\\\\\_____\\/\\\\\\//\\\\\\____/\\\\\\//\\\\\\_      \n   ___\\////\\\\\\_____________\\/\\\\\\_____\\/\\\\\\\\///\\\\\\/\\\\\\/_\\/\\\\\\_     \n    ______\\////\\\\\\__________\\/\\\\\\_____\\/\\\\\\__\\///\\\\\\/___\\/\\\\\\_    \n     _________\\////\\\\\\_______\\/\\\\\\_____\\/\\\\\\____\\///_____\\/\\\\\\_   \n      __/\\\\\\______\\//\\\\\\______\\/\\\\\\_____\\/\\\\\\_____________\\/\\\\\\_  \n       _\\///\\\\\\\\\\\\\\\\\\\\\\/____/\\\\\\\\\\\\\\\\\\\\\\_\\/\\\\\\_____________\\/\\\\\\_ \n        ___\\///////////_____\\///////////__\\///______________\\///__\n\n"

#endif
