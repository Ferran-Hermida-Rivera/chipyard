//========================================================================
// ubmark-bin-search
//========================================================================

#include <stdio.h>
#include "bin-search.dat"

//------------------------------------------------------------------------
// bin_search_scalar
//------------------------------------------------------------------------

__attribute__ ((noinline))
void bin_search_scalar( int values[], int keys[], int keys_sz,
                        int kv[], int kv_sz )
{
    int i;
    for ( i = 0; i < keys_sz; i++ ) {

        int key     = keys[i];
        int idx_min = 0;
        int idx_mid = kv_sz/2;
        int idx_max = kv_sz-1;

        int done = 0;
        values[i] = -1;
        do {
            int midkey = kv[idx_mid];

            if ( key == midkey ) {
                values[i] = idx_mid;
                done = 1;
            }

            if ( key > midkey )
                idx_min = idx_mid + 1;
            else if ( key < midkey )
                idx_max = idx_mid - 1;

            idx_mid = ( idx_min + idx_max ) / 2;

        } while ( !done && (idx_min <= idx_max) );

    }
}

//------------------------------------------------------------------------
// verify_results
//------------------------------------------------------------------------

int verify_results( int values[], int ref[], int size )
{
    int i;
    for ( i = 0; i < size; i++ ) {
        if ( !( values[i] == ref[i] ) ) {
            return 1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------
// Test harness
//------------------------------------------------------------------------

int main()
{

    printf("Start bin search Test.\n");

    int read = 0;
    int write = 3;
    asm volatile(
        "csrw 0x814, %1\n"
        "csrr %0, 0x814"
        : "=r" (read)
        : "r" (write)
    );
    printf("read back from the CSR: %x\n", read);

    int size = 10;
    int values[size];

    int i;
    for ( i = 0; i < size; i++ )
        values[i] = 0;


    bin_search_scalar( values, keys, size, kv, kv_sz );
    int result = verify_results(values, ref, size);
    if (result)
        printf("test failed\n");
    else
        printf("test passed\n");

    read = 0;
    write = 13;
    asm volatile(
        "csrw 0x814, %1\n"
        "csrr %0, 0x814"
        : "=r" (read)
        : "r" (write)
    );
    printf("read back from the CSR: %x\n", read);

    for ( i = 0; i < size; i++ )
        values[i] = 0;


    bin_search_scalar( values, keys, size, kv, kv_sz );
    result = verify_results(values, ref, size);
    if (result)
        printf("test failed\n");
    else
        printf("test passed\n");





    // printf("Start bin search Test part 2.\n");

    // size = 10;

    // for ( i = 0; i < size; i++ )
    //     values[i] = 0;

    // bin_search_scalar( values, keys, size, kv, kv_sz );
    // result = verify_results(values, ref, size);
    // if (result)
    //     printf("test failed\n");
    // else
    //     printf("test passed\n");

    return 0;

}