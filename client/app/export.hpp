#ifndef UUID_2257CCA8_B521_48A2_9C8C_637927811EE8
#define UUID_2257CCA8_B521_48A2_9C8C_637927811EE8

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_FUBBLE
    #ifdef __GNUC__
      #define FUBBLE_PUBLIC __attribute__ ((dllexport))
    #else
      #define FUBBLE_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define FUBBLE_PUBLIC __attribute__ ((dllimport))
    #else
      #define FUBBLE_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define FUBBLE_LOCAL
#else
  #if __GNUC__ >= 4
    #define FUBBLE_PUBLIC __attribute__ ((visibility ("default")))
    #define FUBBLE_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define FUBBLE_PUBLIC
    #define FUBBLE_LOCAL
  #endif
#endif

#endif
