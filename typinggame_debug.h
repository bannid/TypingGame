/* date = May 22nd 2023 9:54 pm */

#ifndef TYPINGGAME_DEBUG_H
#define TYPINGGAME_DEBUG_H
#ifdef DEBUG
#define ASSERT(expression) if(!(expression)) *(int*)0=0
#else
#define ASSERT(expression)
#endif


#endif //TYPINGGAME_DEBUG_H
