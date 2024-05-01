#ifndef OIZYS_API_HPP
#define OIZYS_API_HPP

#ifdef OIZYS_EXPORT

#ifdef __GNUC__
#define OIZYS_API __attribute__ ((visibility ("default")))
#else
#error Compiler not supported.
#endif

#else
#define OIZYS_API
#endif

#endif
