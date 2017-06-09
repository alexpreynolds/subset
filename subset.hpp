#ifndef SUBSET_H_
#define SUBSET_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif /* getline() support */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#include <string>
#include <vector>
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cinttypes>
#include <cstring>
#include <pthread.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <exception>

namespace subset
{
    class Subset
    {
        
    private:

    public:
        bool start_line_index_specified;
        bool number_of_lines_to_retrieve_specified;
        uint32_t start_line_index;
        uint32_t number_of_lines_to_retrieve;
        std::vector<uint32_t> line_numbers;
        std::string line_numbers_fn;
        std::string input_fn;
        std::string output_fn;
        bool prefix_output_with_indices;
        
        static const std::string client_name;
        static const std::string client_version;
        static const std::string client_authors;

        void populate_line_numbers(void);
        void parse_input_file(void);
        void parse_line_numbers_file(void);
        void initialize_command_line_options(int argc, char** argv);
        std::string client_subset_opt_string(void);
        struct option* client_subset_long_options(void);
        std::string client_subset_name(void);
        std::string client_subset_version(void);
        std::string client_subset_authors(void);
        std::string client_subset_usage(void);
        std::string client_subset_description(void);
        std::string client_subset_io_options(void);
        std::string client_subset_general_options(void);
        void print_usage(FILE* wo_stream);
        void print_version(FILE* wo_stream);        
        
        Subset();
        ~Subset();
    };

    Subset::Subset() {
    }
    
    Subset::~Subset() {
    }    
}

#endif // SUBSET_H_
