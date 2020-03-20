#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<math.h>

struct cacheBlock{
	unsigned int valid;
	unsigned long int tag;
	unsigned int time;
	struct cacheBlock *next;
};

int memoryReadsNoPrefetch = 0;
int memoryWritesNoPrefetch = 0;
int cacheHitsNoPrefetch = 0;
int cacheMissesNoPrefetch = 0;
int memoryReadsPrefetch = 0;
//int memoryWritesPrefetch = 0;
int cacheHitsPrefetch = 0;
int cacheMissesPrefetch = 0;

int powerOfTwo(int num){
	if(num==0){
		return 0;
	}
	while(num!=1){
		if(num%2!=0){
			return 0;
		}
		num=num/2;
	}
	return 1;
}

struct cacheBlock** allocateCache(int sets, int n){
//printf("Aa\n");
	struct cacheBlock** temp = malloc(sizeof(struct cacheBlock) * sets);
//printf("Ab\n");
	for (int i = 0; i < sets; i++){
//printf("Ac\n");
		temp[i] = malloc(sizeof(struct cacheBlock) * n);
//printf("Ad\n");
			for (int j = 0; j < n; j++) {
//printf("Ae\n");
				temp[i][j].valid=0;
//printf("Af\n");
			}
//printf("Ag\n");
	}
//printf("Ah\n");
	return temp;
}

void readNoPrefetch(struct cacheBlock** cache, unsigned long tag, int index, int sets, int n){
	int flag=0;
	for(int i=0; i < n; i++){
		if(cache[index][i].valid==1 && cache[index][i].tag==tag){
			flag=1;
			cacheHitsNoPrefetch++;
			break;
		}
	}
	if(flag==1){
		return;
	}else{
		cacheMissesNoPrefetch++;
		memoryReadsNoPrefetch++;
		int full = 1;
        	for(int i=0; i<n; i++){
                	if(cache[index][i].valid==1){
                        	cache[index][i].time++;
                	}
                	if(cache[index][i].valid==0){
                        	full = 0;
                        	break;
                	}
        	}
        	int max=0;
        	if(full == 1){
                	for(int i=0; i<n; i++){
                        	if(cache[index][i].time > max){
                                	max = cache[index][i].time;
                        	}
                	}
                	for(int j=0; j<n; j++){
                        	if(cache[index][j].time==max){
                                	cache[index][j].valid=0;
                                	cache[index][j].tag=(-1);
                                	cache[index][j].time=0;
                                	break;
                        	}
                	}
        	}
		for(int i=0; i < n; i++){
			if(cache[index][i].valid==0){
				cache[index][i].valid=1;
				cache[index][i].tag=tag;
			}
		}
	}
}			

void writeNoPrefetch(struct cacheBlock** cache, unsigned long tag, int index, int sets, int n){
        int flag = 0;
        for(int i = 0; i < n; i++){
		if(cache[index][i].valid==1 && cache[index][i].tag == tag){
			flag = 1;
			cacheHitsNoPrefetch++;
                        memoryWritesNoPrefetch++;
			break;
                }
        }
        if(flag==1){
                return;
        }else{
	        readNoPrefetch(cache, tag, index, sets, n);
		memoryWritesNoPrefetch++;
                return;
        }
}

void prefetch(struct cacheBlock** cache, unsigned long int address, int prefetch_size, int n, int blockSize, int setBits, int offsetBits){
        int flag=0;
        unsigned long int prefetch_address = address;
        for (int i = 0; i < prefetch_size; i++){
                prefetch_address = prefetch_address + blockSize;
                int Mask = (1 << setBits) - 1;
                unsigned int set_index = (prefetch_address >> offsetBits) & Mask;
                unsigned long tag = (prefetch_address >> offsetBits) >> setBits;
                for(int j = 0; j < n; j++){
                        if(cache[set_index][j].valid==1 && cache[set_index][j].tag==tag){
                       		flag=1;
                        	break;
			}
                }
                if(flag==1){
                        return;
                }else{
			memoryReadsPrefetch++;
               		int full = 1;
                	for(int i=0; i<n; i++){
                        	if(cache[set_index][i].valid==1){
                                		cache[set_index][i].time++;
                        	}
                        	if(cache[set_index][i].valid==0){
                                		full = 0;
                                		break;
                        	}
               	 	}
               		int max=0;
                	if(full == 1){
                       		for(int i=0; i<n; i++){
                               		if(cache[set_index][i].time > max){
						max = cache[set_index][i].time;
                              		}
                       		}
                        	for(int j=0; j<n; j++){
                               		if(cache[set_index][j].time==max){
                                      		cache[set_index][j].valid=0;
                                        	cache[set_index][j].tag=(-1);
                                        	cache[set_index][j].time=0;
                                        	break;
                               		}
                       		}	
               		}
                	for(int i=0; i < n; i++){
                        	if(cache[set_index][i].valid==0){
                               		cache[set_index][i].valid=1;
                                	cache[set_index][i].tag=tag;
        //                        	memoryReadsPrefetch++;
                        	}
                	}
		}
        }
}


void readPrefetch(struct cacheBlock** cache, unsigned long tag, int index, int sets, int n, int prefetch_size, int blockSize, int offsetBits, int setBits, unsigned long int address){
        int flag=0;
	for(int i=0; i < n; i++){
                if(cache[index][i].valid==1 && cache[index][i].tag==tag){
                        flag=1;
                       cacheHitsPrefetch++;
                        break;
                }
        }
        if(flag==1){
		//cacheHitsPrefetch++;
                return;
        }else{
		cacheMissesPrefetch++;
		memoryReadsPrefetch++;
              int full = 1;
              for(int i=0; i<n; i++){
                        if(cache[index][i].valid==1){
                                cache[index][i].time++;
                        }
                        if(cache[index][i].valid==0){
                                full = 0;
                                break;
                        }
                }
                int max=0;
                if(full == 1){
                        for(int i=0; i<n; i++){
                                if(cache[index][i].time > max){
                                        max = cache[index][i].time;
                                }
                        }
                        for(int j=0; j<n; j++){
                                if(cache[index][j].time==max){
				        cache[index][j].valid=0;
                                        cache[index][j].tag=(-1);
                                        cache[index][j].time=0;
                                        break;
                                }
                        }
                }
		for(int i=0; i < n; i++){
                        if(cache[index][i].valid==0){
                                cache[index][i].valid=1;
                                cache[index][i].tag=tag;
                        }
                }
		prefetch(cache, address, prefetch_size, n, blockSize, setBits, offsetBits);
	}
}

void writePrefetch(struct cacheBlock** cache, unsigned long tag, int index, int sets, int n, int prefetch_size, int blockSize, int offsetBits, int setBits, unsigned long int address){
        int flag = 0;
        for(int i = 0; i < n; i++){
                if(cache[index][i].valid==1 && cache[index][i].tag == tag){
                        flag = 1;
			cacheHitsPrefetch++;
                        break;
                }
        }
        if(flag==1){
		//cacheHitsPrefetch++;
                return;
        }else{
		readPrefetch(cache, tag, index, sets, n, prefetch_size, blockSize, offsetBits, setBits, address);
		return;
        }
}

int main(int argc, char** argv){
//error checks
        if (argc != 7){
                printf("error1\n");
                exit(0);
        }
	FILE* fp = fopen(argv[6], "r");
  	if(fp == NULL){
    		printf("error2\n");
    		exit(0);
  	}
	int cacheSize = atoi(argv[1]);
	int blockSize = atoi(argv[2]);
	int prefetchSize = atoi(argv[5]);
	int n = 0;
	int setNumber = 0;
	if(powerOfTwo(cacheSize)==0 || powerOfTwo(blockSize==0)){
                printf("error3\n");
                return 0;
        }

	/*if(strcmp(argv[4], "fifo")!=0 || strcmp(argv[4], "lru")!=0){
		printf("error\n");
		return 0;
	}else if(strcmp(argv[4], "lru")==0){
		return 0;
	}*/

	if(strcmp(argv[4], "direct")==0){
		n = 1;
		setNumber = cacheSize/blockSize;
	}else if(strcmp(argv[4], "assoc")==0){
		n = cacheSize/blockSize;
		setNumber = 1;
	}else if(strncmp(argv[4], "assoc:", 6)==0){
		sscanf(argv[4], "assoc:%d", &n);
		if(powerOfTwo(n)==0){
			printf("error5\n");
			return 0;
		}
		setNumber = cacheSize/(blockSize*n);
	}else{
		printf("error4\n");
		return 0;
	}
//all inputs are valid
	struct cacheBlock** noPrefetchCache = allocateCache(setNumber, n);
	struct cacheBlock** prefetchCache = allocateCache(setNumber, n);
	int offsetBits = (int) (log(blockSize)/log(2));
	int setBits = (int) (log(setNumber)/log(2));
	unsigned long int address = 0;
	char readOrWrite = ' ';
	int Mask = (1 << setBits) - 1;
	while((fscanf(fp, "%c %lx\n", &readOrWrite, &address)!=EOF && readOrWrite!='#')){
		unsigned int set_index = (address >> offsetBits) & Mask;
		unsigned long tag = (address >> offsetBits) >> setBits;
		if(readOrWrite=='R'){
			readNoPrefetch(noPrefetchCache, tag, set_index, setNumber, n);
			readPrefetch(prefetchCache, tag, set_index, setNumber, n, prefetchSize, blockSize, offsetBits, setBits, address);
		}else if(readOrWrite=='W'){
			writeNoPrefetch(noPrefetchCache, tag, set_index, setNumber, n);	
			writePrefetch(prefetchCache, tag, set_index, setNumber, n, prefetchSize, blockSize, offsetBits, setBits, address); 
		}
	}
	fclose(fp);

	printf("no-prefetch\n");
	printf("Memory reads: %d\n", memoryReadsNoPrefetch);
	printf("Memory writes: %d\n", memoryWritesNoPrefetch);
	printf("Cache hits: %d\n", cacheHitsNoPrefetch);
	printf("Cache misses: %d\n",  cacheMissesNoPrefetch);
	printf("with-prefetch\n");
	printf("Memory reads: %d\n", memoryReadsPrefetch);
	printf("Memory writes: %d\n", memoryWritesNoPrefetch);
	printf("Cache hits: %d\n", cacheHitsPrefetch);
	printf("Cache misses: %d\n",  cacheMissesPrefetch);
}


