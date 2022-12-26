#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <assert.h>

int is_prime_naive(long p) 
{
    if (p == 2) {
        return 1;
    }
    int i;
    for(i=2; i< p/2; i++) {
        if (p%i == 0) {
            return 0;
        }
    }
    return 1;
}

long modpow_naive(long a, long m, long n)
{
    long res = 1;
    int i;
    for(i=0; i<m; i++){
        res= res*a;
        res = res % n;
    }
    return res;
}


long modpow(long a, long m, long n) 
{
    if(m == 1) {
        return a%n;
    }
    long res;
    long b = modpow(a,m/2,n);
    if(m & 1) {
        res = (a*b*b)%n;
        return res;
    } else {
        res = (b*b)%n;
        return res;
    }
}

int witness ( long a, long b, long d, long p) 
{
    long x = modpow (a,d,p);
    if(x == 1){
        return 0;
    }
    long i;
    for(i = 0; i< b; i++){
        if(x == p -1){
        return 0;
        }
        x = modpow (x ,2,p);
    }
    return 1;
}

long rand_long ( long low , long up)
{
    return rand () % (up - low +1)+ low;
}

int is_prime_miller ( long p, int k) 
{
    if (p == 2) {
        return 1;
    }
    if (!( p & 1) || p <= 1) { //verifier que p est impair et != 1
        return 0;
    }

    //pour b et d :
    long b = 0;
    long d = p - 1;
    while (!( d & 1)) {
        d = d/2;
        b=b+1;
    }

    long a;
    int i;
    for(i = 0; i < k; i++){
        a = rand_long (2, p -1);
        if( witness (a,b,d,p)){
            return 0;
        }
    }
    return 1;
}


long random_prime_number(int low_size, int up_size, int k) 
{
    int random;
    int is_prime = 0;
    int min = pow (2,low_size-1);
    int max = pow (2,up_size)-1;
    
    while( !is_prime ) {
        random = rand_long(min,max);
        is_prime = is_prime_miller(random,k);
    }
    return random;
}

int bit_size(long n) 
{
    int ref = n;
    int size = 1;
    while( ref>1 ) {
        ref = ref/2;
        size++;
    }
    return size;
}

long extended_gcd(long s,long t, long *u, long *v) 
{
    if (s == 0) {
        *u = 0;
        *v = 1;
        return t;
    }
    long uPrim, vPrim;
    long gcd = extended_gcd(t%s, s, &uPrim, &vPrim);
    *u = vPrim - (t/s)*uPrim;
    *v = uPrim;
    return gcd;
}

void generate_key_values(long p, long q, long* n, long* s, long* u) 
{
    *n = p*q;
    long t = (p-1)*(q-1);
    *s = rand_long(1,t-1);
    *u = 0;
    long v = 0;
    int pgcd = 0;
    while( (pgcd != 1) || ((*s) * (*u) % t != 1) ){
        *s = rand_long(1,t-1);
        pgcd = extended_gcd(*s,t,u,&v);
    }
}

// malloc used ! ! ! ! ! ! ! ! 
long* encrypt(char* chaine, long s, long n) 
{
    int m;
    long* c = (long*) malloc(strlen(chaine)*sizeof(long));
    int i = 0;
    while (chaine[i] != '\0') {
        m = (int) chaine[i];
        c[i] = modpow(m,s,n);
        i++;
    }
    return c;
}

// malloc used ! ! ! ! ! ! ! !
char* decrypt(long* crypted, int size, long u, long n) 
{
    long m;
    char* ascii_2_decrypt = (char*) malloc((size)*sizeof(char));
    int i;
    for(i=0; i<size; i++) {
        long c = crypted[i];
        m = modpow(c,u,n);
        ascii_2_decrypt[i] = (char) m;
    }
    ascii_2_decrypt[size] = '\0';
    return ascii_2_decrypt;
}

void print_long_vector(long* result, int size) 
{
    printf("\n[\t");
    int i;
    for(i=0; i<size; i++) {
        printf("%ld \t", result[i]);
    }
    printf("]\n\n");
}


