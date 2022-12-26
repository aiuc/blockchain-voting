#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "premier.h"

int main(void) {
    srand(time(0));
    
    //testing : is_prime_naive

    // parametres des tests: 
    float sec = 0;
    int i = 3;
    int max_prime_found = 3;
    clock_t tic, toc;
    double time_max = 1.0/2000.0;
    
    while(sec < time_max) {
        tic = clock();
        int p = is_prime_naive(i);
        toc = clock();
        sec = ((toc-tic)*1.0)/CLOCKS_PER_SEC*1.0;
        if(p) max_prime_found = i;
        i = i +2;
    }
    printf("\n => le plus grand nombre premier trouve en (1/2000)s : %d \n\n", max_prime_found);
    
    //test + comparaison du temps de calculs : modpow_naive et modpow
    FILE *mp_n = fopen("temps_modpow_naive.txt","w");
    FILE *mp = fopen("temps_modpow.txt","w");

    // parametres des tests: 
    int m = 1;
    int step = 4;
    int nb = 50;
    long double time;
    long x, y;

    while( m<2000 ) {
        time = 0;

        //average of 'nb' measurements of mod_pow_naive 
        for(i=0; i<nb; i++) {
            tic = clock();
            x = modpow_naive(117,m,119);
            toc = clock();
            time += (long double)(toc - tic) / CLOCKS_PER_SEC;
        }
        fprintf(mp_n,"%Lf \n",time/nb);
        
        time = 0;
        
        //average of 'nb' measurements of mod_pow
        for(i=0; i<nb; i++) {
            tic = clock();
            y = modpow(117,m,119);
            toc = clock();
            time += (long double)(toc - tic) / CLOCKS_PER_SEC;
        }
        fprintf(mp,"%Lf \n",time/nb);

        assert(x==y);
        m = m + step;
    }
    fclose(mp_n);
    fclose(mp);
    
    //testing : random_prime_number
    int t_min = 6;
    int t_max = 10;
    int k = (int) ( 0.25*(pow(2,t_max)-1) ) + 1;

    for(int i=0; i<10; i++) {
        int num = random_prime_number(t_min,t_max,k);
        assert( is_prime_naive(num) );
        printf("[ %d ] --> bit size : %d  ",num,bit_size(num));
        printf("\n");
    }
    printf("\n");
    
    printf("=========================================================== \n");
    
    //testing : generate_key_values
    t_min = 3;
    t_max = 7;
    k = 5000;
    long p = random_prime_number(t_min,t_max,k);
    long q = random_prime_number(t_min,t_max,k);
    while (p==q) q = random_prime_number(t_min,t_max,k);
    long n,s,u;
    generate_key_values(p,q,&n,&s,&u);
    if (u<0){
        long t = (p-1)*(q-1);
        u = u+t; //on aura toujours s*u mod t = 1 
    }
    printf("\np = %ld et q = %ld | n : %ld , s : %ld , u : %ld \n",p,q,n,s,u);
    printf("\npublic key : (%ld , %ld) \nprivate key : (%ld , %ld) \n\n",s,n,u,n);

    //testing : encrypt + decrypt
    char message[1000] = "this is the initial message";
    int len = strlen(message);
    
    long* crypted = encrypt(message,s,n);
    
    printf("Initial message : { %s } \n", message);
    
    
    printf("\nEncoded representation : \n");
    print_long_vector(crypted,len);

    char* decoded = decrypt(crypted,len,u,n);
    
    printf("Decoded : { %s }\n\n", decoded);

    free(crypted);
    free(decoded);

    return 0;
}

