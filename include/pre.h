#define strncpy		__kstrncpy
#define strcpy		__kstrcpy
#define strcat		__kstrcat
#define strncat		__kstrncat
#define strlcat		__kstrlcat
#define strlcpy		__kstrlcpy
#define errno		__kerrno
#define loff_t		__kloff_t
#define dev_t		__kdev_t
#define timer_t		__ktimer_t
#define int64_t		__kint64_t
#define u_int64_t	__ku_int64_t
#define sigset_t	__ksigset_t
#define fd_set		__kfd_set
#define blkcnt_t	__kblkcnt_t
#define TRACE_EVENT(name, ...) static inline void trace_##name(...) {}
