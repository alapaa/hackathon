#ifndef _UTILS_H_
#define _UTILS_H_

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define FILE_LINE __FILE__ ":" TOSTRING(__LINE__)

#endif
