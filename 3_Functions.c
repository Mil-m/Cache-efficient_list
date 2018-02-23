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

#ifndef LENMIN
#define LENMIN    1000
#endif

#ifndef LENMAX
#define LENMAX  1000000
#endif

#ifndef LEN
#define LEN      1000
//#define LEN2    2
// LEN2 = log2( LEN )
#endif

#ifndef LENMAS
#define LENMAS  1001
#endif

struct Biglist {
    int elem[LEN];
    int prev[LEN];
    int next[LEN];
};

struct Root {
    int prev;
    int next;
};

int     p_max = 0;
struct Biglist *p[LENMAS]; // = {
//    &(struct Biglist) {
//        {10,10,10,10},
//        {-1,0,1,2},
//        {1,2,3,4},
//    },
//    &(struct Biglist) {
//        {10,10,10,0},
//        {3,5,5,6},
//        {5,6,7,-1},
//    }
//};                          // первый индекс - номер блока, второй - номер элемента в массивах

struct Root empty = { -1, -1 };
struct Root root = { -1, -1 };

static inline int* P_NEXT( int id, struct Root *pr )
{
    return id == -1 ? &(pr->next) : &(p[id/LEN]->next[id%LEN]);
}
static inline int* P_PREV( int id, struct Root *pr )
{
    return id == -1 ? &(pr->prev) : &(p[id/LEN]->prev[id%LEN]);
}
static inline int* P_ELEM( int id )
{
    return id == -1 ? (int*)0 : &(p[id/LEN]->elem[id%LEN]);
}

#define ELEM(n) (*P_ELEM(n))
#define NEXT(n,pr) (*P_NEXT(n,pr))
#define PREV(n,pr) (*P_PREV(n,pr))

int IS_EMPTY(struct Root *pr) {
    return pr->prev == -1 && pr->next == -1 ? 1 : 0;
}

int ENDEMPTY() {
    return IS_EMPTY( &empty );
}

/*
    returns index of first item of new Biglist  OR  -1 on error
*/

int NEWMALLOC()            // добавление элемента в массив указателей на структуры
{
    if ( ENDEMPTY() ) {
        int i, n = LEN*p_max;
        if ( p_max >= LENMAS ) {
            printf ("Haven't free elements");
            return -1;
        }
        struct Biglist *tmp = (struct Biglist*)malloc( sizeof(struct Biglist) );
        if ( !tmp ) {
            printf ("Problems with heap");
            return -2;
        }
            for ( i = 0; i<LEN; i++ ) {
                tmp->elem[i] = 0;
                tmp->prev[i] = n + i - 1;
                tmp->next[i] = n + i + 1;
            }
            tmp->prev[0] = -1;
            tmp->next[LEN-1] = -1;
            p[p_max++] = tmp;
            empty.next = n;
            empty.prev = LEN + n - 1;
    }
    int buf = empty.next;
    empty.next = NEXT(buf, &empty);
    PREV(NEXT(buf, &empty), &empty) = -1;
    return buf;
}

void DELETE( int ind ) {
    if ( ind == -1 ) return;
    struct Root dummy = { -1, -1 };
    int p = PREV( ind, &root ), n = NEXT( ind, &root );
    ELEM(ind) = 0;
    PREV(n, &root) = p;
    NEXT(p, &root) = n;

    PREV(ind, &dummy) = -1;
    NEXT(ind, &dummy) = n = empty.next;
    empty.next = ind;
    PREV(n, &empty) = ind;
    return;
}

void PRINT(int ind, int kol) {
//    int     i, j;

//    kol += ind - 1;
//    for ( i=ind-1; i < kol; i++ ) {
//        printf("\n");
//        for ( j=0; j<LEN; j++ ) {
//            printf( "%d %d %d\n", p[i]->elem[j], p[i]->prev[j], p[i]->next[j] );
//        }
//    }
//    printf("\n");
//-------------

    int ind_first = (ind-1)*LEN;
    int ind_last = ind_first + kol * LEN;
    int i;
    struct Root dummy = { -1, -1 };

    printf( "\nempty .prev=%d, .next=%d\n", empty.prev, empty.next );
    for ( i=ind_first; i<ind_last; i++ ) {
            if ( i%LEN == 0) printf("\n");
            printf( "%d: e=%d p=%d n=%d\n", i, ELEM(i), PREV(i, &dummy ), NEXT(i, &dummy ) );

    /* если LEN - степень двойки, LEN == 1<<LEN2 */
 // printf( "%d %d %d\n", p[i>>LEN2]->elem[i&(LEN-1)], p[i>>LEN2]->prev[i&(LEN-1)], p[i>>LEN2]->next[i&(LEN-1)] );
    }
    printf("\n");
}
int PUSHAFTER(int ind, int key) {
    //int n;
    if ( key == -1 ) return -1;
    NEXT(key, &root) = NEXT(ind, &root);
    PREV(NEXT(ind, &root), &root) = key;
    PREV(key, &root) = ind;
    NEXT(ind, &root) = key;

    //ELEM( ind ) = keypush;
    //n = NEXT( ind, &root ) = root.next;
    //PREV( ind, &root ) = -1;
    //root.next = ind;
    //PREV( n, &root ) = ind;
/*( existing->h.next = root->next )->prev = &existing->h;
    existing->h.prev = root;
    root->next = &existing->h;*/
    return key;
}

int SEARCH(int key) {
    int i;
    struct Root dummy = { -1, -1 };

    for ( i = root.next; i != -1; i = NEXT( i, &dummy ) ) {
        if (ELEM(i) == key) {
            return i;
        }
    }
    return -1;
}

int u[ LENMAX+1 ];

int main() {

#ifdef WIN32
    long long tt, ttmp;
    #define TM()    (QueryPerformanceCounter( (LARGE_INTEGER*)&ttmp ), ((double)ttmp)/tt)
#else
    struct timespec tt;
    #define TM()    (clock_gettime( CLOCK_THREAD_CPUTIME_ID, &tt ), (tt.tv_sec+1.e-9*tt.tv_nsec))
#endif
    double t0, t1, t2, t3, t4;
    int N;
    int c = 0;
    //int errs;

#ifdef WIN32
    QueryPerformanceFrequency( (LARGE_INTEGER*)&tt );
#endif

    int i, buf;
    //int ind = 1; // с какой печатать? (1, 2,..., n) (включая)
    //int kol = LENMAS; // до какой печатать? (1, 2,..., n) (включая)

    for (N=LENMIN; N<=LENMAX; N+=(N/5?N/5:1)) {

#ifndef SKIP_TIME
        t0 = TM();
#endif

#ifndef SKIP_TIME
        t1 = TM();
#endif

        u[0] = -1;
        for ( i=1; i <= N; i++ ) {
            buf = NEWMALLOC();
            int r = rand()%(i);
            ELEM( buf ) = i;
            u[i] = PUSHAFTER(u[r], buf);	//вставка в произвольный
        }

#ifndef SKIP_TIME
        t2 = TM();
#endif

        int yes = 0, no = 0;
#ifndef SKIP_SEARCH
        for ( i=1; i < N; i++ ) {
            int r = rand()%i;
            buf = SEARCH(r);
            if (buf != -1) {
                yes ++;
            } else {
                no ++;
            }
        }

        //printf("%d %d", yes, no);
#endif

#ifndef SKIP_TIME
        t3 = TM();
#endif

        c = 0;
        while ( !IS_EMPTY( &root ) ) {
            c ++;
            DELETE( root.next );
        }

#ifndef SKIP_TIME
        t4 = TM();
        printf("%d; %G; %G; %G; %d\n", N, t2-2*t1+t0, t3-t2-t1+t0, t4-t3-t1+t0, N - c);
#endif
    }
    //PRINT(1, p_max);
    return 0;
}
