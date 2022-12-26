
int is_prime_naive(long p);
long modpow_naive(long a, long m, long n);

long modpow(long a, long m, long n);
int witness ( long a, long b, long d, long p);
long rand_long ( long low , long up);
int is_prime_miller ( long p, int k);
long random_prime_number(int low_size, int up_size, int k);

int bit_size(long n);
void generate_key_values(long p, long q, long* n, long* s, long* u);
long* encrypt(char* chaine, long s, long n);
char* decrypt(long* crypted, int size, long u, long n);
void print_long_vector(long* result, int size);