
#include "TeaBot/TeaBot.hpp"

#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <errno.h>


typedef struct _account_list
{
    uint32_t    api_id;
    const char  *api_hash;
    const char  *data_path;
} account_list;


static void *thread_worker(void *p_);
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

    std::thread *threads = new std::thread[n_list];

    for (uint32_t i = 0; i < n_list; i++) {
        threads[i] = std::thread(thread_worker, (void *)&(list[i]));
    }

    for (uint32_t i = 0; i < n_list; i++) {
        threads[i].join();
    }

    delete[] threads;
    free(list);

    return 0;
}


/**
 * @param void *p_
 * @return void *
 */
static void *thread_worker(void *p_)
{
    account_list *list = (account_list *)p_;

    printf("api_id    = %d\n", list->api_id);
    printf("api_hash  = %s\n", list->api_hash);
    printf("data_path = %s\n", list->data_path);

    TeaBot::TeaBot bot(list->api_id, list->api_hash, list->data_path);
    bot.run();

    free((void *)list->api_hash);
    free((void *)list->data_path);
    list->api_hash  = NULL;
    list->data_path = NULL;

    return NULL;
}


/**
 * @param const char *file
 * @param uint32_t   *n_list_p
 * @return account_list *
 */
static account_list *load_account_list(const char *file, uint32_t *n_list_p)
{
    char          *p;
    char          acc_buf[4096];
    uint32_t      n_list        = 0;
    FILE          *acc_handle   = NULL;
    account_list  *list         = NULL;

    acc_handle = fopen(file, "r");
    if (acc_handle == NULL) {
        printf("Error fopen(\"%s\"): %s\n", file, strerror(errno));
        goto clean_up;
    }

    while ((p = fgets(acc_buf, sizeof(acc_buf), acc_handle)) != NULL) {
        account_list    *tmp;
        uint32_t        api_id;
        FILE            *handle;
        char            *p2;
        char            *api_hash;
        char            name_buf[5120];
        char            file_buf[1024];
        size_t          len = strlen(acc_buf);

        if (acc_buf[len - 1] == '\n')
            acc_buf[len - 1] = '\0';

        n_list++;
        tmp = (account_list *)realloc(list, sizeof(account_list) * n_list);
        if (tmp == NULL) {
            free(list);
            list = NULL;
            printf("Error realloc(): %s\n", strerror(errno));
            goto clean_up;
        }
        list = tmp;


        /*
         * Load API ID
         */
        sprintf(name_buf, "%s/api_id.txt", acc_buf);
        handle = fopen(name_buf, "r");
        if (handle == NULL) {
            free(list);
            list = NULL;
            printf("Error fopen(\"%s\"): %s\n", name_buf, strerror(errno));
            goto clean_up;
        }

        p2 = fgets(file_buf, sizeof(file_buf), handle);
        if (p2 == NULL) {
            free(list);
            list = NULL;
            printf("Unable to read api_id from \"%s\"\n", file_buf);
            goto clean_up;
        }
        fclose(handle);
        if ((api_id = strtoul(file_buf, NULL, 10)) == 0) {
            printf("Invalid api_id from \"%s\"\n", file_buf);
            goto clean_up;
        }



        /*
         * Load API Hash
         */
        sprintf(name_buf, "%s/api_hash.txt", acc_buf);
        handle = fopen(name_buf, "r");
        if (handle == NULL) {
            free(list);
            list = NULL;
            printf("Error fopen(\"%s\"): %s\n", name_buf, strerror(errno));
            goto clean_up;
        }

        p2 = fgets(file_buf, sizeof(file_buf), handle);
        if (p2 == NULL) {
            printf("Unable to read api_hash from \"%s\"\n", file_buf);
            goto clean_up;
        }
        fclose(handle);
        len = strlen(file_buf);

        if (file_buf[len - 1] == '\n')
            file_buf[len - 1] = '\0';

        api_hash = strdup(file_buf);


        list[n_list - 1].api_id    = api_id;
        list[n_list - 1].api_hash  = api_hash;
        list[n_list - 1].data_path = strdup(acc_buf);
    }


clean_up:

    if (acc_handle != NULL)
        fclose(acc_handle);

    if (list != NULL)
        *n_list_p = n_list;

    return list;
}
