#ifndef COM_API_HPP
#define COM_API_HPP

#ifdef COM_EXPORT

#ifdef __GNUC__
#define COM_API __attribute__ ((visibility ("default")))
#else
#error Compiler not supported.
#endif

#else
#define COM_API
#endif

#endif
