#include "stdinc.h"
#include "db_parser.h"
#include "uint128_t.h"
#include <network>
#include <vector>
#include <sstream>

namespace ip = std::experimental::net::ip;

// read filters from input file
// return the number of fields in the first filter
int read_filters(FilterList *filters, FILE *fp_in) {
    int line_matches = 0;
    int matches = 0;
    char *line_buffer = new char[500];
    char string_buffers[20][50];
    int buffer_size = 500;
    uint128_t temp_addr[16];
    int d = 5;
    struct filter temp_filter;
    int dtemp;

    for (int i = 0; i < MAXFILTERS; i++) {
        dtemp = 0;
        // initialize filter entry
        temp_filter.sa_len = temp_filter.da_len = 0;
        temp_filter.sp[0] = temp_filter.sp[1] = 0;
        temp_filter.dp[0] = temp_filter.dp[1] = 0;
        temp_filter.prot_num = 0;
        temp_filter.flags = temp_filter.flags_mask = 0;
        temp_filter.num_ext_field = 0;

        // read in line
        line_buffer = fgets(line_buffer, buffer_size, fp_in);
        if (line_buffer == NULL) return d;

        // parse line into strings
        line_matches = sscanf(line_buffer, "@%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
                              string_buffers[0] , string_buffers[1] , string_buffers[2] , string_buffers[3] , 
                              string_buffers[4] , string_buffers[5] , string_buffers[6] , string_buffers[7] , 
                              string_buffers[8] , string_buffers[9] , string_buffers[10], string_buffers[11], 
                              string_buffers[12], string_buffers[13], string_buffers[14], string_buffers[15], 
                              string_buffers[16], string_buffers[17], string_buffers[18], string_buffers[19]);

        // scan through strings to find fields
        if (line_matches > 4) {
            // return to standard IPv6 address
            std::string source = standard_ipv6(string_buffers[0]);
            std::string destination = standard_ipv6(string_buffers[1]);
            std::string ipv6_part;
            // std::cout << source << std::endl << destination << std::endl;

            // source address
            int pos = 0;
            for (int part = 0; part < 16; part++) {  
                ipv6_part = "";
                ipv6_part.push_back(source[2*part + pos]);
                ipv6_part.push_back(source[2*part + pos + 1]);
                if (part%2 == 1) pos++;
                // std::cout << ipv6_part << std::endl;
                temp_addr[part] = std::stoi(ipv6_part, nullptr, 16);
                temp_addr[part] <<= (15-part)*8;
                // std::cout << temp_addr[part] << " part " << part << std::endl;
            }
            temp_filter.sa = temp_addr[0] | temp_addr[1] | temp_addr[2] | temp_addr[3] |
                             temp_addr[4] | temp_addr[5] | temp_addr[6] | temp_addr[7] |
                             temp_addr[8] | temp_addr[9] | temp_addr[10]| temp_addr[11]|
                             temp_addr[12]| temp_addr[13]| temp_addr[14]| temp_addr[15];
            for (int part = 40; part < (int)source.size(); part++) {
                temp_filter.sa_len += pow(10, (int)source.size()-1-part) * (source[part]-'0');
            } dtemp++;
            // std::cout << temp_filter.sa << " / " << temp_filter.sa_len << std::endl;
            
            // destination address
            pos = 0;
            for (int part = 0; part < 16; part++) {
                ipv6_part = "";
                ipv6_part.push_back(destination[2*part + pos]);
                ipv6_part.push_back(destination[2*part + pos + 1]);
                if (part%2 == 1) pos++;
                temp_addr[part] = std::stoi(ipv6_part, nullptr, 16);
                temp_addr[part] <<= (15-part)*8;
            }
            temp_filter.da = temp_addr[0] | temp_addr[1] | temp_addr[2] | temp_addr[3] |
                             temp_addr[4] | temp_addr[5] | temp_addr[6] | temp_addr[7] |
                             temp_addr[8] | temp_addr[9] | temp_addr[10]| temp_addr[11]|
                             temp_addr[12]| temp_addr[13]| temp_addr[14]| temp_addr[15];
            for (int part = 40; part < (int)destination.size(); part++) {
                temp_filter.da_len += pow(10, (int)destination.size()-1-part) * (destination[part]-'0');
            } dtemp++;
            // std::cout << temp_filter.da << " / " << temp_filter.da_len << std::endl;


            // source ports
            // scan string buffer 2 for decimal source port value, low
            matches = sscanf(string_buffers[2], "%d", &temp_filter.sp[0]);
            // check the number of matches
            if (matches != 1) {
	            fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No low source port specification.\n");
            } // std::cout << "source ports from " << temp_filter.sp[0];
            // check for ":
            if (string_buffers[3][0] != ':') {
	            fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No : source port specification.\n");
            }
            // scan string buffer 4 for decimal source port value, high
            matches = sscanf(string_buffers[4], "%d", &temp_filter.sp[1]);
            // check the number of matches
            if (matches != 1) {
	            fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No high source port specification.\n");
            } // std::cout << " to " << temp_filter.sp[1] << std::endl;
            dtemp++;

            // destination ports
            matches = sscanf(string_buffers[5], "%d", &temp_filter.dp[0]);
            if (matches != 1) {
	            fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No low destination port specification.\n");
            } // std::cout << "destination ports from " << temp_filter.dp[0];
            if (string_buffers[6][0] != ':') {
	            fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No : destination port specification.\n");
            }
            matches = sscanf(string_buffers[7], "%d", &temp_filter.dp[1]);
            if (matches != 1) {
                fprintf(stderr, "Parser: error, partial filter entry at filter %d.\n", i);
                fprintf(stderr, "No high destination port specification.\n");
            } // std::cout << " to "  << temp_filter.dp[1] << std::endl;
            dtemp++;

            // protocol
            int prot_mask = 0;
            // scan string buffer 8 for protocol
            matches = sscanf(string_buffers[8], "0x%02x/0x%02x", &temp_filter.prot_num, &prot_mask);
            if (matches != 2) {
	            fprintf(stderr, "Parser: bad protocol spec at filter %d.\n", i);      
            } // std::cout << "protocol " << temp_filter.prot_num << ", mask " << prot_mask << std::endl;
            dtemp++;

            // extra fields
            if (line_matches >= 10) {
                // initialize flags and extra fields
                temp_filter.flags = temp_filter.flags_mask = 0;
                temp_filter.num_ext_field = 0;
            	temp_filter.ext_field = nullptr;
                char x1, x2;

                // read flags if present
                matches = sscanf(string_buffers[9], "0x%04x/0x%04x", &temp_filter.flags, &temp_filter.flags_mask);
                if (matches == 2) {
                    dtemp++;
                } else {
                    fprintf(stderr, "Parser: bad flags spec at filter %d.\n", i);
                }
            }

            if (line_matches >= 11) {
                // allocate extra field array
                temp_filter.ext_field = new int[line_matches-10];
                temp_filter.num_ext_field = line_matches-10;

                // search for additional fields
                for (int j = line_matches-11; j >= 0; j--) {
	                // scan string buffer for decimal field value, 0 = wildcard
	                matches = sscanf(string_buffers[line_matches-j-1], "%d", &temp_filter.ext_field[j]);
	                // check the number of matches
	                if (matches != 1) {
	                    fprintf(stderr, "Parser: error: invalid extra field in filter entry %d.\n", i);
	                }
	                dtemp++;
	            }
            }

            // add filter to list
            (*filters) &= temp_filter;
            if (dtemp > d) d = dtemp;
        } else {
            fprintf(stderr, "Parser: error, invalid filter at entry %d.\n", i);
            i--;
        }
    }   

    delete(line_buffer);
    return d;
}

std::string standard_ipv6(char ipv6_char[]) {
    std::string ipv6_str; 
    for (int i = 0; i < strlen(ipv6_char); i++) {
        ipv6_str.push_back(ipv6_char[i]);
    }

    // deal with "/"
    size_t pos = ipv6_str.find('/');
    if (pos != std::string::npos) {
        ipv6_str = ipv6_str.substr(0, pos);
    }

    // deal with "::"
    size_t double_colon_pos = ipv6_str.find("::");
    bool has_double_colon = (double_colon_pos != std::string::npos);
    
    std::vector<std::string> segments;
    std::stringstream ss(ipv6_str);
    std::string segment;
    
    if (has_double_colon) {
        std::string before = ipv6_str.substr(0, double_colon_pos);
        std::string after = ipv6_str.substr(double_colon_pos + 2);
        // before ss
        std::stringstream ss_before(before);
        while (std::getline(ss_before, segment, ':')) {
            if (!segment.empty()) segments.push_back(segment);
        }
        // after ss
        std::stringstream ss_after(after);
        while (std::getline(ss_after, segment, ':')) {
            if (!segment.empty()) segments.push_back(segment);
        }
        
        int missing_segments = 8 - segments.size();
        for (int i = 0; i < missing_segments; ++i) {
            segments.push_back("0000");
        }
    } else {
        while (std::getline(ss, segment, ':')) {
            if (!segment.empty()) segments.push_back(segment);
        }
    }

    for (auto& seg : segments) {
        while (seg.length() < 4) {
            seg = "0" + seg;
        }
    }

    std::ostringstream result;
    for (size_t i = 0; i < segments.size(); ++i) {
        result << segments[i];
        if (i != segments.size() - 1) result << ":";
    }

    return result.str();
}
