#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pthread.h>
#include "regexp.h"
#include "tslinkedlist.h"
#include "tsorderedset.h"
#include "tsiterator.h"

#define USAGE "file_crawler pattern [directory] ...\nN.B. pattern must be enclosed in \'\'"

const TSLinkedList *ll = NULL;
const TSOrderedSet *os = NULL;
const RegExp *rex = NULL;
int active_threads = 0;

pthread_mutex_t lock;
pthread_cond_t cond;

static int cmpfxn (void *a, void *b) {
	/*
	 * compare function to be used inside ordered set
	 */
	return strcmp ((char *)a, (char *)b);
}

char *bash_to_re (char *re, char *ans) {
	/*
	 * helper function to convert bash to a regex
	 * this function assumes `ans' buffer is large enough to hold regex
	 */

	int len = strlen(re);
	int i;
	for (i = 0; i < len; i++) {
		if (re[i] == '*')
			(void) strcat(ans, ".*");
		else if (re[i] == '.')
			(void) strcat(ans, "\\.");
		else if (re[i] == '?')
			(void) strcat(ans, ".");
		else{
			char c[2];
			c[0] = re[i];
			c[1] = '\0';
			(void) strcat(ans, c);
		}
	}
	(void) strcat(ans, "$");
	return ans;
}

char *addSlash (char *str, char *buf) {
	(void) strcpy(buf, str);
	int len = strlen (str);
	if (str[len-1] == '/')
		return buf;

	(void) strcat(buf, "/");
	return buf;
}

int process_dir (char *str) {
	char path[512];
	(void) addSlash(str, path);
	DIR *dir;
	struct dirent *entry;
	dir = opendir(path);
	if (dir) {
		while ((entry = readdir(dir))) {
			if (entry->d_type == DT_DIR){
				
				/* ignore `.' and `..' directories */
				if (strcmp (entry->d_name, ".") != 0 && strcmp (entry->d_name, "..") != 0) {
					char new_path[512];
					(void) strcpy(new_path, path);
					(void) strcat(new_path, entry->d_name);
					char *new_dir = strdup (new_path);
					if (!new_dir)
						return 0;

					if (!(ll->addLast(ll, new_dir)))
						return 0;

					/* more work added */
					pthread_mutex_lock(&lock);
					active_threads++;
					pthread_mutex_unlock(&lock);
				}
			}
			else if (rex->match(rex, entry->d_name)) {
				char new_path[512];
				(void) strcpy(new_path, path);
				(void) strcat(new_path, entry->d_name);
				char *new_dir = strdup (new_path);
				if (!new_dir)
					return 0;

				if (!(os->add(os, new_dir)))
					return 0;
			}
		}
		closedir(dir);
		
	}
	else 
		fprintf(stderr, "Unable to open `%s' directory\nNo such directory[s]\n", path);
	
	return 1;
}

void *run () {
	/*
	 * run by the threads
	 */

	while (ll->size(ll)) {
		pthread_mutex_lock(&lock);
		active_threads--;
		pthread_mutex_unlock(&lock);

		while (active_threads && ll->size(ll) == 0)
			pthread_cond_wait(&cond, &lock);

		char *path;
		(void)ll->removeFirst(ll, (void **) &path);

		if (!process_dir(path)){
			fprintf(stderr, "[MALLOC FAILURE] failed to add new path\n");
			return NULL;
		}

		pthread_cond_broadcast(&cond);
	}

	return NULL;
}

void clean_up () {
	if (ll != NULL)
		ll->destroy(ll, free);
	if (os != NULL)
		os->destroy(os, free);
	if (rex != NULL)
		rex->destroy(rex);
}

int main (int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s\n", USAGE);
		exit(0);
	}

	rex = RegExp_create();
	if (rex == NULL) {
		fprintf(stderr, "[MALLOC FAILURE] failed to create regex object\n");
		exit(0);
	}

	int nthreads = 2;
	char *CRAWLER_THREADS = getenv("CRAWLER_THREADS");
	if(CRAWLER_THREADS != NULL){
		nthreads = atoi(CRAWLER_THREADS);
		if(nthreads<1 || nthreads>50){
			fprintf(stderr, "CRAWLER_THREADS is either too large or too small (<1 or >50): %d\nUsing 2 threads\n", nthreads);
			nthreads = 2;
		}
	}
	printf("%d\n", nthreads);

	active_threads = nthreads;
	pthread_t tids[nthreads];

	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond, NULL);

	/* start collecting directories */

	char pattern[256];
	bash_to_re(argv[1], pattern);
	if (rex->compile(rex, pattern)) {

		ll = TSLinkedList_create();
		if (ll != NULL) {

			os = TSOrderedSet_create(cmpfxn);
			if (os != NULL) {

				if (argc == 2) {
					char *cur_dir = strdup(".");
					if (!cur_dir)
						goto end;

					if (!(ll->addLast (ll, (void *)cur_dir)))
						goto end;
				}
				else {

					int i;
					for (i = 2; i < argc; i++){
						char *dir = strdup(argv[i]);
						if (!dir)
							goto end;

						if (!(ll->addLast (ll, (void *)dir)))
							goto end;
					}
				}
			}
			else 
				goto end;
		}
		else 
			goto end;
	}
	else 
		goto end;

	/* finished collecting directories */

	int i;
	for (i = 0; i < nthreads; i++)
		if (pthread_create(&tids[i], NULL, run, NULL) != 0)
			goto end;

	/*join threads*/
	for(i = 0; i < nthreads; i++)
		if(pthread_join(tids[i], NULL) != 0)
			fprintf(stderr, "Failed to join threads\n");
	

	const TSIterator *it = os->itCreate(os);
	if (it != NULL) {

		while (it->hasNext(it)) {

			char *element;
			it->next(it, (void **) &element);
			printf("%s\n", element);
		}
		it->destroy(it);
	}

end:
	clean_up();
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	exit(0);
}



