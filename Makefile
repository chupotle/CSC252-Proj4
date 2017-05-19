FILES := src/cache.c src/trace.c src/hashtable.c

cacheSim: $(FILES)
	gcc -o $@ $^ -O3

cacheSimDebug: $(FILES)
	gcc -o $@ $^ -g

clean:
	rm -f cacheSimDebug
	rm -f cacheSim

.PHONY : clean
