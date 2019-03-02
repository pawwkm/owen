#include "parser.h"
#include "semantics.h"

int main(int argc, const char* argv[])
{
    parserTestSuite();
    semanticsTestSuite();

    return 0;
}
