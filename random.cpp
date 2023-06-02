#include <time.h>
#include <stdlib.h>

void
RandomInitialize()
{
	srand(time(NULL));
}
f32 
Random()
{
	f32 RandomBwZeroAndOne = (f32) rand() / RAND_MAX;
	return (RandomBwZeroAndOne);
}