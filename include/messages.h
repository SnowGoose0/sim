#ifndef MESSAGES_H
#define MESSAGES_H

#define ARGS "neh"

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

#endif
