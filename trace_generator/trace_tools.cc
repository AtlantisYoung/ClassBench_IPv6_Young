#include "stdinc.h"
#include "dlist.h"
#include "hlist.h"
#include "trace_tools.h"

// generate headers
int header_gen(int d, FilterList *filters, FILE *fp, float a, float b, int scale) {
    int num_headers = 0;
    int fsize = filters->size();
    int threshold = scale * fsize;

    struct header *new_hdr = new header;
    // generate headers
    while (num_headers < threshold) {
        // pick a random filter
        int RandFilt = randint(1, fsize);
        // pick a random corner of the filter for a header
        random_corner(RandFilt, filters, new_hdr);
        // select number of copies to add to header list from Pareto distribution
        int copies = my_pareto(a, b);

        for (int i = 0; i < copies; i++) {
            if (new_hdr->sa.upper() == 0 && new_hdr->da.upper() == 0) {
                fprintf(fp, "%lu\t%lu\t",
                        new_hdr->sa.lower(), new_hdr->da.lower());
            } else if (new_hdr->sa.upper() != 0 && new_hdr->da.upper() == 0) {
                fprintf(fp, "%lu%020lu\t%lu\t",
                        new_hdr->sa.upper(), new_hdr->sa.lower(), new_hdr->da.lower());
            } else if (new_hdr->sa.upper() == 0 && new_hdr->da.upper() != 0) {
                fprintf(fp, "%lu\t%lu%020lu\t",
                        new_hdr->sa.lower(), new_hdr->da.upper(), new_hdr->da.lower());
            } else {
                fprintf(fp, "%lu%020lu\t%lu%020lu\t",
                        new_hdr->sa.upper(), new_hdr->sa.lower(), new_hdr->da.upper(), new_hdr->da.lower());
            }

            fprintf(fp, "%d\t%d\t%d\t%d\n",
                    new_hdr->sp,
                    new_hdr->dp,
                    new_hdr->prot_num,
                    new_hdr->answer);
        } num_headers += copies;
    } delete(new_hdr);

    return num_headers;
}

void random_corner(int RandFilt, FilterList *filters, struct header *new_hdr) {
    int d = TUPLE; double p;
    FilterList_item *filter = (*filters)(RandFilt);
    struct filter filter_t = filter->filt;

    p = drand48();
    new_hdr->sa = (p < 0.5) ?
                  low(filter_t.sa, filter_t.sa_len) :
                  high(filter_t.sa, filter_t.sa_len);
    // std::cout << filter_t.sa << "  " << filter_t.sa_len << std::endl;
    // std::cout << "sa: " << new_hdr->sa << "\n" << std::endl;

    p = drand48();
    new_hdr->da = (drand48() < 0.5) ?
                  low(filter_t.da, filter_t.da_len) :
                  high(filter_t.da, filter_t.da_len);
    // std::cout << filter_t.da << "  " << filter_t.da_len << std::endl;
    // std::cout << "da: " << new_hdr->da << "\n" << std::endl;
    
    new_hdr->sp = randint(filter_t.sp[0], filter_t.sp[1]);
    new_hdr->dp = randint(filter_t.dp[0], filter_t.dp[1]);
    new_hdr->prot_num = filter_t.prot_num;

    new_hdr->answer = linear_search(filters, new_hdr);

    return;
}

int my_pareto(float a, float b) {
    // calculate the Pareto Probability
    if (b == 0) return 1;

    double p;
    p = drand48();
    double x = (double)b / pow((double)(1-p), (double)(1/(double)a));
    int num = (int)ceil(x);

    return num;
}

int linear_search(FilterList *filters,struct header *hdr) {
    int fsize = filters->size();
    for (int j = 1; j <= fsize; j++) {
        FilterList_item *filter_t = (*filters)(j);
        if (hdr->sa <= high(filter_t->filt.sa, filter_t->filt.sa_len) &&
            hdr->sa >= low(filter_t->filt.sa, filter_t->filt.sa_len) &&
            hdr->da <= high(filter_t->filt.da, filter_t->filt.da_len) &&
            hdr->da >= low(filter_t->filt.da, filter_t->filt.da_len) &&
            hdr->sp >= filter_t->filt.sp[0] &&
            hdr->sp <= filter_t->filt.sp[1] &&
            hdr->dp >= filter_t->filt.dp[0] &&
            hdr->dp <= filter_t->filt.dp[1] &&
            hdr->prot_num == filter_t->filt.prot_num) {
            return j;
        }
    }

    return -1;
}

uint128_t high(uint128_t addr, int len) {
    uint128_t addr_t;
    uint128_t addr_i(0xffffffffffffffff, 0xffffffffffffffff);
    addr_t = (addr >> (ADDRLEN-len)) << (ADDRLEN-len);
    addr_i = addr_i >> len;
    
    return (addr_t + addr_i);
}

uint128_t low(uint128_t addr, int len) {
    uint128_t addr_t;
    addr_t = (addr >> (ADDRLEN-len)) << (ADDRLEN-len);

    return addr_t;
}
