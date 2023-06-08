#include <algorithm>
#include <charconv>
#include <cstring>
#include <iostream>
#include <iterator>
#include <vector>

#include <omp.h>

bool ParseCLMatrixSize( char* arg, std::pair< int, int >& matrix_size )
{
    char* begin = arg, * end = arg + std::strlen( arg );

    char* delimiter = std::strchr( arg, 'x' );

    if( delimiter == nullptr )
        return false;

    {
        auto err = std::from_chars( begin, delimiter, matrix_size.first );

        if( err.ec != std::errc() )
            return false;

        if( err.ptr != delimiter )
            return false;
    }

    {
        auto err = std::from_chars( delimiter + 1, end, matrix_size.second );

        if( err.ec != std::errc() )
            return false;

        if( err.ptr != end )
            return false;
    }

    return true;
}

bool ParseCLThreads( char* arg, int& threads )
{
    char* begin = arg, * end = arg + std::strlen( arg );

    auto err = std::from_chars( begin, end, threads );

    if( err.ec != std::errc() )
        return false;

    if( err.ptr != end )
        return false;

    return true;
}

int main( int argc, char *argv[] )
{
    int threads = 1;
    std::pair< int, int > first_matrix_size;
    std::pair< int, int > second_matrix_size;

    if( argc < 4 ||
            !ParseCLMatrixSize( argv[1], first_matrix_size ) ||
            !ParseCLMatrixSize( argv[2], second_matrix_size ) ||
            !ParseCLThreads( argv[3], threads ) ||
            first_matrix_size.second != second_matrix_size.first )
    {
        std::cout << "Matrix MxN NxK\n";
        std::cout << "M - count of row\n";
        std::cout << "N - count of column\n";
        std::cout << "N - count of row\n";
        std::cout << "K - count of column\n";
        std::cout << "n - count of threads\n";
        std::cout << std::endl;

        return 0;
    }

    std::vector< int > first_matrix;
    first_matrix.reserve( first_matrix_size.first * first_matrix_size.second );

    std::vector< int > second_matrix;
    second_matrix.reserve( second_matrix_size.first * second_matrix_size.second );

    std::vector< int > result_matrix;
    result_matrix.resize( first_matrix_size.first * second_matrix_size.second );

    std::cout << "Enter first matrix [" << first_matrix_size.first << "x" << first_matrix_size.second << "]" << std::endl;
    std::copy_n( std::istream_iterator< int >( std::cin ), first_matrix_size.first * first_matrix_size.second, std::back_inserter( first_matrix ) );

    std::cout << "Enter second matrix [" << second_matrix_size.first << "x" << second_matrix_size.second << "]" << std::endl;
    std::copy_n( std::istream_iterator< int >( std::cin ), second_matrix_size.first * second_matrix_size.second, std::back_inserter( second_matrix ) );

    omp_set_num_threads( threads );

    auto start = omp_get_wtime();

    #pragma omp parallel for
    for( int i = 0; i < first_matrix_size.first; ++i )
    {
        for( int j = 0; j < second_matrix_size.second; ++j )
        {
            for( int k = 0; k < first_matrix_size.first; ++k )
            {
                result_matrix[ i*second_matrix_size.second + j ]
                        += first_matrix[ i*first_matrix_size.second + k ] * second_matrix[ k*second_matrix_size.second + j ];
            }
        }
    }

    auto end = omp_get_wtime();

    std::cout << "Elapsed time: " << (end - start) << std::endl;

    return 0;
}
