#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#endif

#ifndef offsetof
#define offsetof(s,f)   ((size_t)&(((struct s*)0)->f))
#endif

#ifndef NMIN
#define NMIN 1000
#endif

#ifndef NMAX
#define NMAX 1000000
#endif

#ifndef LEN
#define LEN 1000
#endif

struct hdr {
        struct hdr *prev, *next;
};

struct Elem {
    int elem;
    struct hdr  h;
};

inline struct Elem* hdr2elem(struct hdr* ph)
{
        return (struct Elem*) ( ((char*)ph) - offsetof(Elem,h) );
}

struct hdr root = { &root, &root };

struct Elem* Pushafter(struct hdr *list, int x){
        struct Elem *buf = (struct Elem*)malloc(sizeof(struct Elem));

        if ( buf ) {
                buf->elem = x;

                ( buf->h.next = list->next )->prev = &buf->h;
                buf->h.prev = list;
                list->next = &buf->h;
        }
        return buf;
}

struct Elem* Search(int x){
    struct hdr *buf = root.next;

    while ( buf != &root && hdr2elem(buf)->elem != x ) {
        buf = buf->next;
    }

    return ( buf == &root ? (struct Elem*)0 : hdr2elem(buf) );
}

void PrintList(void){
    struct Elem *buf = hdr2elem( root.next );

	while ( buf != hdr2elem(&root) ) {
		printf("%d ", buf->elem);
		buf = hdr2elem( buf->h.next );
	}
    printf("\n");
}

void Delete(struct Elem *list){
    list->h.prev->next = list->h.next;
    list->h.next->prev = list->h.prev;
    free(list);
}

struct hdr* p[ NMAX+1 ];

int main(){
    int x, i = 0;
    struct Elem *buf;
#ifdef WIN32
    long long tt, ttmp;
    #define TM()    (QueryPerformanceCounter( (LARGE_INTEGER*)&ttmp ), ((double)ttmp)/tt)
#else
    struct timespec tt;
    #define TM()    (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &tt ), (tt.tv_sec+1.e-9*tt.tv_nsec))
#endif

#ifndef SKIP_TIME
    double t0, t1, t2, t3, t4;
#endif
    int     errs, N;

#ifdef WIN32
    QueryPerformanceFrequency( (LARGE_INTEGER*)&tt );
#endif
    for ( N=NMIN; N<=NMAX; N+=(N/5?N/5:1) ) {
#ifndef SKIP_TIME
        t0 = TM();
#endif
        //p[0] = &root;
        //for (i = 1; i <= N; i ++) {
        //    p[i] = (struct hdr*)( rand()%i );
        //}

#ifndef SKIP_TIME
        t1 = TM();
#endif
        p[0] = &root;
        for (i = 1; i <= N; i ++) {
            p[i] = &(Pushafter( p[ rand()%i ], i)->h);
        }

#ifndef SKIP_TIME
        t2 = TM();
#endif
        errs = 0;
#ifndef SKIP_SEARCH
        for (i = 1; i < N; i ++) {
            buf = Search( x = rand() % N + 1 );
            if ( !buf || buf->elem != x ) errs++;
        }
#endif
#ifndef SKIP_TIME
        t3 = TM();
#endif
        while ( root.next != &root ) Delete( hdr2elem( root.next ) );
#ifndef SKIP_TIME
        t4 = TM();
        printf("%d; %G; %G; %G; %d\n", N, t2-2*t1+t0, t3-t2-t1+t0, t4-t3-t1+t0, errs );
#endif
    }
	return 0;
}

