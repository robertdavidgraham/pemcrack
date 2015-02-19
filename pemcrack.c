/*
    PEMCRACK

    Cracks password encrypted private keys for SSL.

    This is really slow, since it's using the high-level OpenSSL
    API.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rsa.h>


/*
 * Tell Microsoft's compilers to link to OpenSSL, so that the
 * makefile doesn't need this explicitly.
 */
#if defined(_MSC_VER)
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

/*
 * Track progress
 */
unsigned progress_count = 0;


/*
 * Tests a single password to see if it's the right one. If 
 * it is, then it prints it on the command line and exits the
 * program.
 */
void crackpwd(FILE *fp, char *password)
{
    EVP_PKEY *pkey = 0;
    void *p;

    /* Print regular progress. On a 3-GHz CPU, it's a few
     * seconds between updates. Slower CPUs will be slower */
    if ((progress_count++ & 0x7F) == 0)
        fprintf(stderr, "-> %s\n", password);

    /* Seek to the start of the file before trying a password,
     * since we are using the high-level OpenSSL API */
    fseek(fp, 0, SEEK_SET);

    /* Attempt to read the file using the current password */
    p = PEM_read_PrivateKey(fp,&pkey, NULL, password);
    if (p) {
        /* FOUND!!! print and exit the program */
        printf("found: %s\n", password);
        exit(0);
    }
}

/*
 * Attempt to brute-force the password trying all
 * lower-case letters. Since password cracking is
 * so slow, this is of little use
 */
void bruteforce(FILE *fp, char *password, unsigned depth)
{
    unsigned i;

    for (i=0; i<26; i++) {
        password[depth] = 'a' + i;
                
        if (depth)
            bruteforce(fp, password, depth-1);
        else {
            crackpwd(fp, password);
        }
    }
}

/*
 * pemcrack <pem> <dict>
 */
int main(int argc, char *argv[])
{
    FILE *fp;
    FILE *dict;
    char password[64] = {0};
    char line[2048];

    fprintf(stderr, "--- pemcrack v1.0 - by Robert Graham ----\n");
    if (argc <= 1 || 3 < argc) {
        fprintf(stderr, "Usage:\n pemcrack <pem> <dict>\n");
        exit(1);
    }

    /* have to add crypto algorithms, or the necessary AES 
     * code won't be there to decrypt */
    OpenSSL_add_all_algorithms();

    /* Open the file to read in the PEM file. We leave this file
     * open while cracking, seeking back to the beginning each
     * time. */
    fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror(argv[1]);
        exit(1);
    }

    /*
     * If no dictionary file given, then brute-force
     */
    if (argc <= 2) {
        fprintf(stderr, "no dictionary specified, so brute-forcing...\n");
        bruteforce(fp, password, 0);
        bruteforce(fp, password, 1);
        bruteforce(fp, password, 2);
        bruteforce(fp, password, 4);
        bruteforce(fp, password, 5);
        bruteforce(fp, password, 6);
        return 1;
    }


    /*
     * Open dictionary file of possible passwords
     */
    dict = fopen(argv[2], "rt");
    if (dict == NULL) {
        perror(argv[2]);
        exit(1);
    }

    /*
     * For each password in the file, attempt to crack it
     */
    while (fgets(line, sizeof(line), dict)) {
        /* removing trailing whitespace */
        while (strlen(line) && isspace(line[strlen(line)-1]))
            line[strlen(line)-1] = '\0';

        /* attempt to crack; if found, this will exit the program */
        crackpwd(fp, line);
    }


    fprintf(stderr, "NOT FOUND\n");
    return 1;
}
