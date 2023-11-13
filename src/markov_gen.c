#include <markov.h>
/*****
 * Markov chain generator functions
 *****/
HTable* markov_generate_ht(char *fname) {
    HTable *ht = create_table(CAPACITY);
    FILE *f = fopen(fname, "r+");

    /* Okay, so this needs to do the following:
     * - Open the dataset file
     * - Read 2 characters in the file (a1a2)
     * - Find every character that
     *   follows those two characters, add it to a CList
     * - Advance to next two characters (a2a3)
     * - Repeat until EOF
     *
     * Maybe this would be a good place to use SList, read the file and store
     * each name in a separate node?
     * - Open the dataset file
     * - Read and store strings in SList
     * - Loop through SList, doing:
     *   = take first two characters of string a1a2
     *   = add next character to CList a3
     *   = Look through rest of SList for any two characters that match
     *   = Add the following character to CList
     *   = Add to ht: key a1a2, CList
     *   = Repeat with next two characters in string a2a3
     *   = Continue until one of the characters is '\0'.
     *   = GO to next string in SList
     */

    fclose(f);
    return ht;
}

/*****
 * Random name functions
 *****/
