
#include <unistd.h>

#include "src/ext/md5_php.h"
#include "src/ext/sha1_php.h"


#include <string.h>

int main(void)
{
	// unsigned char md5_buf[16];
	// md5_file("test.txt", md5_buf);
	// write(1, md5_buf, sizeof(md5_buf));

	unsigned char sha1_buf[20];
	sha1_file("test.txt", sha1_buf);
	write(1, sha1_buf, sizeof(sha1_buf));
}
