
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <errno.h>

#include <TeaBot/TeaBot.hpp>
#include <TeaBot/helpers.hpp>


typedef struct account_ {
	uint32_t api_id;
	char api_hash[64];
	char data_path[1024];
} account;


/**
 * @param const char *data_path
 * @param account *acc_p
 * @return int
 */
__attribute__((always_inline))
inline static int load_account(const char *data_path, account *acc_p)
{
	int retval;
	char buf[2048];
	char con_bf[64]; /* file content buffer */
	FILE *handle;

	retval = 0;

	(strncpy(acc_p->data_path, data_path, sizeof(acc_p->data_path) - 1));

	/* Load API Hash */
	snprintf(buf, sizeof(buf), "%s/api_hash.txt", data_path);
	handle = fopen(buf, "r");
	if (handle == NULL) {
		printf("Error: fopen(\"%s\"): %s\n", buf, strerror(errno));
		retval = -1;
		goto out;
	}

	if (fgets(con_bf, sizeof(con_bf), handle)) {
		(strncpy(acc_p->api_hash, trim(con_bf),
			sizeof(acc_p->api_hash) - 1));
	} else {
		printf("Cannot read API hash from \"%s\"\n", buf);
		retval = -EINVAL;
		goto out;
	}
	fclose(handle);
	/* End of Load API ID */


	/* Load API ID */
	snprintf(buf, sizeof(buf), "%s/api_id.txt", data_path);
	handle = fopen(buf, "r");
	if (handle == NULL) {
		printf("Error: fopen(\"%s\"): %s\n", buf, strerror(errno));
		retval = -1;
		goto out;
	}

	if (fgets(con_bf, sizeof(con_bf), handle)) {
		acc_p->api_id = strtoul(con_bf, NULL, 10);
	} else {
		printf("Cannot read API ID from \"%s\"\n", buf);
		retval = -EINVAL;
		goto out;
	}
	/* End of Load API ID */

out:
	if (handle != NULL)
		fclose(handle);

	return retval;
}


/**
 * @param cons char *filename
 * @param account   **acc
 * @param size_t	*n_acc_p
 * @return int
 */
static int load_account_list(const char *filename, account **acc_list_p,
							 size_t *n_acc_p)
{
	int retval;
	FILE *acc_h;
	char buf[1023];
	account *acc_list = NULL;
	size_t n_acc = 0;
	size_t acc_alloc = 5;

	retval = 0;

	if (acc_list_p == NULL) {
		printf("acc_list_p cannot be NULL\n");
		retval = -EINVAL;
		goto out;
	}

	acc_h = fopen(filename, "r");
	if (acc_h == NULL) {
		printf("Error: fopen(\"%s\"): %s\n", filename, strerror(errno));
		retval = -1;
		goto out;
	}

	acc_list = (account *)malloc(sizeof(account) * acc_alloc);
	if (acc_list == NULL) {
		printf("Error: malloc(): %s\n", strerror(errno));
		retval = -1;
		goto out;
	}

	while (fgets(buf, sizeof(buf), acc_h) != NULL) {
		size_t len;
		int load_retval;

		len = strlen(buf);

		if (buf[len - 1] == '\n')
			buf[--len] = '\0';

		if ((n_acc + 1) >= acc_alloc) {
			account *tmp;

			acc_alloc *= 2;
			tmp = (account *)realloc(acc_list,
						 sizeof(account) * acc_alloc);
			if (tmp == NULL) {
				printf("Error: realloc(): %s\n",
					strerror(errno));
				retval = -1;
				goto out;
			}
			acc_list = tmp;
		}

		load_retval = load_account(buf, &acc_list[n_acc]);
		if (load_retval < 0) {
			retval = load_retval;
			goto out;
		}
		n_acc++;
	}


out:
	if (acc_h != NULL)
		fclose(acc_h);


	if (retval == 0) {

		/* Success. */
		if (n_acc_p != NULL)
			*n_acc_p = n_acc;

		*acc_list_p = acc_list;

	} else {

		/* Error occured. */
		if (acc_list != NULL)
			free(acc_list);

	}

	return retval;
}


/**
 * @param void *_acc
 * @return void *
 */
static void *thread_worker(void *_acc)
{
	account *acc = (account *)_acc;

	printf("api_id    = %d\n", acc->api_id);
    	printf("api_hash  = %s\n", acc->api_hash);
    	printf("data_path = %s\n", acc->data_path);

    	TeaBot::TeaBot bot(acc->api_id, acc->api_hash, acc->data_path);
    	bot.run();

	return NULL;
}


/**
 * @param int  argc
 * @param char *argv[]
 * @return int
 */
int main(int argc, char *argv[])
{
	int retval;
	std::thread *threads = NULL;
	account *acc_list = NULL;
	size_t  n_acc	  = 0;

	if (argc != 2) {
		printf("Usage: %s [account list file]\n", argv[0]);
		return 1;
	}

	if (load_account_list(argv[1], &acc_list, &n_acc) < 0)
		return 1;

	retval = 0;

	try {
		threads = new std::thread[n_acc];

		for (size_t i = 0; i < n_acc; i++) {
			threads[i] = std::thread(thread_worker,
						 (void *)&acc_list[i]);
		}

		for (size_t i = 0; i < n_acc; i++) {
			threads[i].join();
		}

	} catch (...) {
		retval = 1;
		printf("Error occured\n");
	}


	if (acc_list != NULL)
		free(acc_list);

	if (threads != NULL)
		delete[] threads;

	return retval;
}
