#ifndef __FLIST_H_ 
#define __FLIST_H_

class flist {
    int df;               // number of fields in the filters
    int Nf;               // number of filters in the list
    unsigned **lowf;
    unsigned **highf;
public:
    flist(int, int);      // constructor
    ~flist();             // destructor
    int d();              // return number of fields per filter
    int N();              // return capacity of filter list
    unsigned low(int filt, int field);                          // return low bound for filter i, field j
    unsigned high(int filt, int field);                         // return low bound for filter i, field j
    void add(int filt, int field, unsigned low, unsigned high); // assign low and high bounds for filter i, field j
    void print(FILE *fp); // print filter list
};

#endif
