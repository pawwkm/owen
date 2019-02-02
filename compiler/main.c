#include <stdio.h>
#include "parser.h"

#ifdef Test

int main()
{
    parserTestSuite();

    return 0;
}

#else

int main(int argc, const char* argv[]))
{
    printf("Run the Compiler\n");

    return 0;
}

#endif
