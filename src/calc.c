#include "stack.h"
#include "poly.h"
#include "poly_from_text.h"
#include "poly_to_text.h"
#include "instructions_reader.h"
#include <string.h>
#include <sys/types.h>

#define STACK_INIT_SIZE 8

int main(){
    long line_nr = 0;
    char *string = NULL;
    ssize_t bytes_read;
    size_t size = 0;
    Stack *st = makeStack(STACK_INIT_SIZE);

    do {
        line_nr++;
        bytes_read = getline(&string, &size, stdin);
        if (bytes_read == -1) {
            break;
        }

        if (*string == '#' || strcmp(string, "\n") == 0 || strcmp(string, "") == 0) {
            if (strlen(string) < bytes_read) {      //przypadek znaku \0 na początku linii
                fprintf(stderr,"ERROR %ld WRONG POLY\n", line_nr);
            }
            continue;
        }

        if (isLetter(*string)) {
            if (strlen(string) < bytes_read) {
                fprintf(stderr,"ERROR %ld WRONG COMMAND\n", line_nr);
                continue;
            }
            takeInstruction(string, st, line_nr);
        }

        else {
            if (strlen(string) < bytes_read) {
                fprintf(stderr,"ERROR %ld WRONG POLY\n", line_nr);
                continue;
            }
            Stack *poly_stack = makeStack(STACK_INIT_SIZE);
            bool succ = true;
            Poly p = stringToPoly(poly_stack, string, line_nr, &succ);
            if (succ) {
                Element e = elementOfPoly(&p);
                push(st, e);
            }
            destroyStack(poly_stack);
        }
    }
        while (string != NULL);

    destroyStack(st);
    free(string);
    return 0;
}