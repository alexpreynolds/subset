#include "subset.hpp"

const std::string subset::Subset::client_name = "subset";
const std::string subset::Subset::client_version = "1.0";
const std::string subset::Subset::client_authors = "Alex Reynolds";

int
main(int argc, char** argv)
{
    subset::Subset subset;

    subset.initialize_command_line_options(argc, argv);
    if ((subset.start_line_index_specified) && (subset.number_of_lines_to_retrieve_specified)) {
        subset.populate_line_numbers();
    }
    else {
        subset.parse_line_numbers_file();
    }
    subset.parse_input_file();
    
    return EXIT_SUCCESS;
}

void
subset::Subset::populate_line_numbers(void)
{
    for (auto i = this->start_line_index; i < this->start_line_index + this->number_of_lines_to_retrieve; ++i) {
        line_numbers.push_back(i);
    }
}

void
subset::Subset::parse_input_file(void)
{
    char *buf = NULL;
    size_t buf_len = 0;
    ssize_t buf_read = 0;
    FILE* input_stream = NULL;
    uint32_t current_line_number = 0;
    FILE* output_stream = NULL;

    input_stream = this->input_fn.empty() ? NULL : std::fopen(this->input_fn.c_str(), "r");
    if (!input_stream) {
        std::fprintf(stderr, "Error: Input file handle could not be created\n");
        std::exit(ENODATA);
    }

    output_stream = this->output_fn.empty() ? stdout : std::fopen(this->output_fn.c_str(), "w");
    if (!output_stream) {
        std::fprintf(stderr, "Error: Output file handle could not be created\n");
        std::exit(ENODATA);        
    }

    auto i = line_numbers.begin(); 
    uint32_t line_number_to_match = *i;
    
    while (((buf_read = getline(&buf, &buf_len, input_stream)) != EOF) && (i != line_numbers.end())) {
        if (line_number_to_match == current_line_number) {
            if (prefix_output_with_indices) {
                std::fprintf(output_stream, "[%09u] %s", current_line_number, buf);
            }
            else {
                std::fprintf(output_stream, "%s", buf);
            }
            i++;
            if (i == line_numbers.end()) {
                break;
            }
            line_number_to_match = *i;
        }
        current_line_number++;
    }

    std::fclose(input_stream);
    std::fclose(output_stream);
}

void
subset::Subset::parse_line_numbers_file(void)
{
    char *buf = NULL;
    size_t buf_len = 0;
    ssize_t buf_read = 0;
    uint32_t line_number = 0;
    FILE* line_numbers_stream = NULL;

    line_numbers_stream = this->line_numbers_fn.empty() ? NULL : std::fopen(this->line_numbers_fn.c_str(), "r");
    if (!line_numbers_stream) {
        std::fprintf(stderr, "Error: Line number file handle could not be created\n");
        std::exit(ENODATA);
    }

    while ((buf_read = getline(&buf, &buf_len, line_numbers_stream)) != EOF) {
        std::sscanf(buf, "%d\n", &line_number);
        line_numbers.push_back(line_number);
    }

    std::sort(line_numbers.begin(), line_numbers.end());

    std::fclose(line_numbers_stream);
}

std::string
subset::Subset::client_subset_opt_string(void)
{
    static std::string _s("s:n:pl:i:o:hv?");
    return _s;
}

struct option*
subset::Subset::client_subset_long_options(void)
{
    static struct option _t = { "start-index",            required_argument,   NULL,    's' };
    static struct option _n = { "number-of-lines",        required_argument,   NULL,    'n' };
    static struct option _p = { "prefix-with-indices",    no_argument,         NULL,    'p' };
    static struct option _l = { "line-numbers",           required_argument,   NULL,    'l' };
    static struct option _i = { "input",                  required_argument,   NULL,    'i' };
    static struct option _o = { "output",                 required_argument,   NULL,    'o' };
    static struct option _h = { "help",                   no_argument,         NULL,    'h' };
    static struct option _v = { "version",                no_argument,         NULL,    'v' };
    static struct option _0 = { NULL,                     no_argument,         NULL,     0  };
    static std::vector<struct option> _s;
    _s.push_back(_t);
    _s.push_back(_n);    
    _s.push_back(_p);
    _s.push_back(_l);
    _s.push_back(_i);
    _s.push_back(_o);
    _s.push_back(_h);
    _s.push_back(_v);
    _s.push_back(_0);
    return &_s[0];
}

void
subset::Subset::initialize_command_line_options(int argc, char** argv)
{
    int client_long_index;
    int client_opt = getopt_long(argc,
                                 argv,
                                 this->client_subset_opt_string().c_str(),
                                 this->client_subset_long_options(),
                                 &client_long_index);

    this->start_line_index_specified = false;
    this->number_of_lines_to_retrieve_specified = false;
    this->prefix_output_with_indices = false;    

    opterr = 0; /* disable error reporting by GNU getopt */
    
    while (client_opt != -1) {
        switch (client_opt) {
        case 's':
            this->start_line_index_specified = true;
            std::sscanf(optarg, "%d", &this->start_line_index);
            break;
        case 'n':
            this->number_of_lines_to_retrieve_specified = true;
            std::sscanf(optarg, "%d", &this->number_of_lines_to_retrieve);            
            break;
        case 'p':
            this->prefix_output_with_indices = true;
            break;
        case 'l':
            this->line_numbers_fn.assign(optarg);
            break;
        case 'i':
            this->input_fn.assign(optarg);
            break;
        case 'o':
            this->output_fn.assign(optarg);
            break;
        case 'h':
            this->print_usage(stdout);
            std::exit(EXIT_SUCCESS);
        case 'v':
            this->print_version(stdout);
            std::exit(EXIT_SUCCESS);
        case '?':
            this->print_usage(stdout);
            std::exit(EXIT_SUCCESS);
        default:
            break;
        }
        client_opt = getopt_long(argc,
                                 argv,
                                 this->client_subset_opt_string().c_str(),
                                 this->client_subset_long_options(),
                                 &client_long_index);
    }
    
    if (this->input_fn.empty()) {
        std::fprintf(stderr, "Error: Undefined input file!\n");
        this->print_usage(stderr);
        exit(EXIT_FAILURE);
    }

    if ((this->line_numbers_fn.empty()) && (!this->start_line_index_specified)) {
        std::fprintf(stderr, "Error: Undefined line numbers file!\n");
        this->print_usage(stderr);
        exit(EXIT_FAILURE);
    }

    if ((this->start_line_index_specified) && (this->number_of_lines_to_retrieve_specified) && (!this->line_numbers_fn.empty())) {
        std::fprintf(stderr, "Error: Cannot both specify line numbers file and start/number values!\n");
        this->print_usage(stderr);
        exit(EXIT_FAILURE);        
    }

    if (((this->start_line_index_specified) && (!this->number_of_lines_to_retrieve_specified)) || ((!this->start_line_index_specified) && (this->number_of_lines_to_retrieve_specified))) {
        std::fprintf(stderr, "Error: Must specify both start index and number of lines to retrieve!\n");
        this->print_usage(stderr);
        exit(EXIT_FAILURE);                
    }
}

std::string
subset::Subset::client_subset_name(void)
{
    static std::string _s(subset::Subset::client_name);
    return _s;
}

std::string
subset::Subset::client_subset_version(void)
{
    static std::string _s(subset::Subset::client_version);
    return _s;
}

std::string
subset::Subset::client_subset_authors(void)
{
    static std::string _s(subset::Subset::client_authors);
    return _s;
}

std::string
subset::Subset::client_subset_usage(void)
{
    static std::string _s("\n"                                          \
                          "  Usage:\n"                                  \
                          "\n"                                          \
                          "  $ subset [ --line-numbers=<str> | --start-index=<int> --number-of-lines=<int> ]\n" \
                          "             --input=<str>\n" \
                          "           [ --output=<str> ]\n");
    return _s;
}

std::string
subset::Subset::client_subset_description(void)
{
    static std::string _s("  Read (0-indexed) line numbers from file and\n" \
                          "  write out subset of lines from the input file\n" \
                          "  at the desired line numbers to the specified\n" \
                          "  output file or to the standard output stream.\n");
    return _s;
}

std::string
subset::Subset::client_subset_io_options(void)
{
    static std::string _s("  General Options:\n\n"              \
                          "  --start-index=i         Starting line (0-indexed) from which to begin writing lines\n" \
                          "                          from input (integer, optional)\n" \
                          "  --number-of-lines=i     Number of lines to write out after the start index (integer)\n" \
                          "  --line-numbers=s        Line numbers filename (string, optional)\n" \
                          "  --input=s               Input filename (string)\n" \
                          "  --output=s              Output filename (string, optional)\n" \
                          "                          If omitted, output is written to stdout\n\n" \
                          "  You must use either --line-numbers or the --start-index/--number-of-lines pair. You cannot\n" \
                          "  use both options, however.\n");
    return _s;
}

std::string
subset::Subset::client_subset_general_options(void)
{
    static std::string _s("  Process Flags:\n\n"                        \
                          "  --help                  Show this usage message\n" \
                          "  --version               Show binary version\n");
    return _s;
}

void
subset::Subset::print_usage(FILE* wo_stream)
{
    std::fprintf(wo_stream,
                 "%s\n"                                              \
                 "  version: %s\n"                                   \
                 "  author:  %s\n"                                   \
                 "%s\n"                                              \
                 "%s\n"                                              \
                 "%s\n"                                              \
                 "%s\n",
                 this->client_subset_name().c_str(),
                 this->client_subset_version().c_str(),
                 this->client_subset_authors().c_str(),
                 this->client_subset_usage().c_str(),
                 this->client_subset_description().c_str(),
                 this->client_subset_io_options().c_str(),
                 this->client_subset_general_options().c_str());
}

void
subset::Subset::print_version(FILE* wo_stream)
{
    std::fprintf(wo_stream,
                 "%s\n"                                              \
                 "  version: %s\n"                                   \
                 "  author:  %s\n",
                 this->client_subset_name().c_str(),
                 this->client_subset_version().c_str(),
                 this->client_subset_authors().c_str());
}
