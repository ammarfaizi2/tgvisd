
#include "TeaBot/TeaBot.hpp"

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <errno.h>
#include <pthread.h>


typedef struct _account_list
{
    uint32_t    api_id;
    const char  *api_hash;
    const char  *data_path;
} account_list;


static account_list *load_account_list(const char *file, uint32_t *n_list_p);


/**
 * @param int  argc
 * @param char *argv[]
 * @return int
 */
int main(int argc, char *argv[])
{
    account_list *list;
    uint32_t     n_list;

    if (argc != 2) {
        printf("Usage: %s [account list file]\n", argv[0]);
        return 0;
    }

    if ((list = load_account_list(argv[1], &n_list)) == NULL) {
        return 1;
    }

    return 0;
}


/**
 * @param const char *file
 * @param uint32_t   *n_list_p
 * @return account_list *
 */
static account_list *load_account_list(const char *file, uint32_t *n_list_p)
{
    char          *p;
    char          file_buf[4096];
    uint32_t      n_list    = 0;
    FILE          *handle   = NULL;
    account_list  *list     = NULL;

    handle = fopen(file, "r");

    if (handle == NULL) {
        printf("Error fopen(): %s\n", strerror(errno));
        goto clean_up;
    }


    while ((p = fgets(buf, sizeof(buf), stdin)) != NULL) {

        account_list *tmp;
        char         data_buf[256];
        char         name_buf[5120];
        FILE         *acc_handle;
        size_t       len = strlen(buf);

        if (buf[len - 1] == '\n')
            buf[len - 1] = '\0';

        n_list++;

        tmp = realloc(list, sizeof(account_list) * n_list);
        if (tmp == NULL) {
            free(list);
            list = NULL;
            printf("Error realloc(): %s\n", strerror(errno));
            goto clean_up;
        }
        list = tmp;
    }


clean_up:

    if (handle != NULL)
        fclose(handle);

    if (list != NULL)
        *n_list_p = n_list;

    return list;
}
