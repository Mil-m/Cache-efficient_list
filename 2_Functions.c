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

typedef
struct List {
    int elem;
    struct hdr  h;
}  list_t;

struct hdr root = { &root, &root };
struct hdr empty = { &empty, &empty };

inline list_t* hdr2elem(struct hdr* ph)    //³- ³-³³TT‚³-T… ³-³-T€³-T‰³³³-³¿T ³³ T„Tƒ³-³³T†³¿³¿ ³³³-³-³¿³¿³¿TT‚³-T€ ³-TT‚³-³-³¿T‚ T‚³³³¿³- TT‚³-³³ T„Tƒ³-³³T†³¿³¿, ³- ³-³³ ³³³-³+ ³-T‹³¿³-³-³-
{
        return (list_t*) ( ((char*)ph) - offsetof(List,h) );   //³-T‹T‡³¿T³¿T³³T‚TT T³-³³T‰³³³-³¿³³ (³- ³-³-³³T‚³-T…) - T‚.³³. ³-³³ ³-³-TŠT³-³¿. 1-T‹³- ³- ³¿³-³¿³³ Elem
}

int IS_EMPTY(struct hdr *p) {
    if (p->next == p) {
        return 1;
    }
    return 0;
}

void INIT(list_t *root) {
    return;
}

struct hdr* FIRST(struct hdr *p) {
    return IS_EMPTY(p) ? (struct hdr *)0 : p->next;
}

struct hdr* LAST(struct hdr *p) {
    return IS_EMPTY(p) ? (struct hdr *)0 : p->prev;
}

list_t* SEARCH(int key) {
    struct hdr *buf = FIRST( &root );
    while ( buf && buf != &root && hdr2elem(buf)->elem != key ) {
        buf = buf->next;
    }
    return ( buf == &root ? (list_t*)0 : hdr2elem(buf) );
}

list_t* NEWMALLOC(void){
    int i;
    if (IS_EMPTY(&empty)) {
        list_t *mas = (list_t*)malloc(sizeof(list_t)*LEN);
        for (i = 0; i < LEN; i ++) {
            mas[i].h.next = &(mas[i + 1].h);
            mas[i].h.prev = &(mas[i - 1].h);
        }
        mas[0].h.prev = &empty;
        empty.next = &(mas[0].h);
        mas[i - 1].h.next = &empty;
        empty.prev = &(mas[i - 1].h);
    }
    struct hdr *buf = empty.next;
    empty.next = empty.next->next;
    empty.next->prev = empty.next;
    return hdr2elem(buf);
}

list_t* PUSHAFTER(struct hdr* root, list_t* existing){
    ( existing->h.next = root->next )->prev = &existing->h;
    existing->h.prev = root;
    root->next = &existing->h;
    return existing;
}

list_t* ADD(list_t* existing, int key) {
    list_t* p = (list_t*)malloc(sizeof(list_t)*LEN);
    p->elem = key;
    PUSHAFTER(&(p->h), existing);
    return p;
}

int DELETE(list_t* root){
    root->h.prev->next = root->h.next;
    root->h.next->prev = root->h.prev;
    PUSHAFTER( empty.prev, root );
    return 1;
}

struct hdr* p[ NMAX+1 ];

int main(){
    int x = 0;
    int i = 0;
    list_t* buf;
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
//	p[0] = &root;
//        for (i = 1; i <= N; i ++) {
//            p[i] = (struct hdr*)( rand()%i );
//        }

#ifndef SKIP_TIME
       t1 = TM();
#endif
        p[0] = &root;
        for (i = 1; i <= N; i ++ ) {
            list_t *buf = (list_t*)NEWMALLOC();
            int t = rand()%i;
	    buf->elem = i;
//            if (t % 2 != 0) {
//                t --;
//            }
            p[i] = &(PUSHAFTER( p[t] , buf )->h);
        }

#ifndef SKIP_TIME
        t2 = TM();
#endif

        errs = 0;
#ifndef SKIP_SEARCH
        for (i = 1; i < N; i ++ ) {
            int t = rand() % N + 1;
            buf = SEARCH( t );
            if ( !buf || buf->elem != t ) errs++;
//            if (t % 2 == 0) {
//                if ( buf && buf->elem != x) errs ++;
//            } else {
//                if ( buf ) errs ++;
//            }
        }
#endif

#ifndef SKIP_TIME
        t3 = TM();
#endif
            while ( !IS_EMPTY( &root ) ) DELETE( hdr2elem( FIRST(&root) ) );

#ifndef SKIP_TIME
        t4 = TM();
        printf("%d; %G; %G; %G; %d\n", N, t2-2*t1+t0, t3-t2-t1+t0, t4-t3-t1+t0, errs );
#endif
    }
	return 0;
}
