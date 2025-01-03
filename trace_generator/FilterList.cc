#include "stdinc.h"
#include "FilterList.h"
#include <network>

namespace ip = std::experimental::net::ip;

FilterList::FilterList() {
  first = last = NULL;
  num = 0;
}

FilterList::~FilterList() { 
    struct FilterList_item *temp;
    while (first != NULL) {
        if (first->filt.num_ext_field > 0) {
            delete[] (first->filt.ext_field);
        }
        temp = first->next;
        delete(first);
        first = temp;
    }
}

// remove all elements
void FilterList::clear() {
    struct FilterList_item *temp;
    while (first != NULL) {
        temp = first->next;
        delete(first);
        first = temp;
    }
    return;
}

// return the i-th element, where the first is 1
struct FilterList_item* FilterList::operator()(int i) {
    if (i <= 0 || (i > num && num > 0)) {
        fprintf(stderr, "FilterList::operator(): item %d out of range, num = %d\n", i, num);
        exit(1);      
    }
    struct FilterList_item *temp;
    temp = first;
    while (--i) {
        temp = temp->next;
    }
    return temp;
}

// insert filter prior to the given element
void FilterList::insert(struct FilterList_item *item, struct filter filt) {
    struct FilterList_item *newitem;
    newitem = new struct FilterList_item;
    copy_filter(newitem->filt, filt);
    newitem->prev = item->prev;
    newitem->next = item;
    if (first == item) {
        first = newitem;
    } else {
        item->prev->next = newitem;
    }
    item->prev = newitem;
    num++;
    return;
}

// add i to the end of the FilterList
void FilterList::operator&=(struct filter filt) {
    struct FilterList_item *temp;
    temp = new struct FilterList_item;
    copy_filter(temp->filt, filt);
    temp->prev = last;
    temp->next = NULL;
    if (num == 0) {
        first = temp;
    } else {
        last->next = temp;
    }
    last = temp;
    num++;
    return;
}

// remove the first i filters
void FilterList::operator<<=(int i) {
    struct FilterList_item *temp;
    while (first != NULL && i--) {
        temp = first->next;
        delete(first);
        first = temp;
        num--;
    }
    return;
}

// assign value of right operand to left operand
void FilterList::operator=(FilterList* L) {
    clear();
    struct FilterList_item *temp;
    for (temp = (*L)(1); temp != NULL; temp = temp->next)
        (*this) &= temp->filt;
    return;
}

// add i to the front of the FilterList
void FilterList::push(struct filter filt) {
    struct FilterList_item *temp;
    temp = new struct FilterList_item;
    copy_filter(temp->filt, filt);
    temp->next = first;
    temp->prev = NULL;
    if (num == 0) {
        last = temp;
    } else {
        first->prev = temp;
    }
    first = temp;
    num++;
    return;
}

// print the contents of the FilterList
void FilterList::print(FILE* fp) {
    uint128_t temp;
    struct FilterList_item *tempfilt;
    
    for (tempfilt = first; tempfilt != NULL; tempfilt = tempfilt->next) {
        // new filter character
        fprintf(fp, "@");
        // source address
        temp = tempfilt->filt.sa;
        const ip::address_v6::bytes_type src_temp_bytes_type(
            (unsigned char) (temp.upper() >> 56),
            (unsigned char) (temp.upper() >> 48),
            (unsigned char) (temp.upper() >> 40),
            (unsigned char) (temp.upper() >> 32),
            (unsigned char) (temp.upper() >> 24),
            (unsigned char) (temp.upper() >> 16),
            (unsigned char) (temp.upper() >>  8),
            (unsigned char) (temp.upper()      ),
            (unsigned char) (temp.lower() >> 56),
            (unsigned char) (temp.lower() >> 48),
            (unsigned char) (temp.lower() >> 40),
            (unsigned char) (temp.lower() >> 32),
            (unsigned char) (temp.lower() >> 24),
            (unsigned char) (temp.lower() >> 16),
            (unsigned char) (temp.lower() >>  8),
            (unsigned char) (temp.lower()      ));
        ip::address_v6 src_addr(src_temp_bytes_type);
        fprintf(fp, "%s/%d\t", src_addr.to_string().c_str(), tempfilt->filt.sa_len);
        
        // destination address
        temp = tempfilt->filt.da;
        const ip::address_v6::bytes_type dst_temp_bytes_type(
            (unsigned char) (temp.upper() >> 56),
            (unsigned char) (temp.upper() >> 48),
            (unsigned char) (temp.upper() >> 40),
            (unsigned char) (temp.upper() >> 32),
            (unsigned char) (temp.upper() >> 24),
            (unsigned char) (temp.upper() >> 16),
            (unsigned char) (temp.upper() >>  8),
            (unsigned char) (temp.upper()      ),
            (unsigned char) (temp.lower() >> 56),
            (unsigned char) (temp.lower() >> 48),
            (unsigned char) (temp.lower() >> 40),
            (unsigned char) (temp.lower() >> 32),
            (unsigned char) (temp.lower() >> 24),
            (unsigned char) (temp.lower() >> 16),
            (unsigned char) (temp.lower() >>  8),
            (unsigned char) (temp.lower()      ));
        ip::address_v6 dst_addr(dst_temp_bytes_type);
        fprintf(fp, "%s/%d\t", dst_addr.to_string().c_str(), tempfilt->filt.da_len);

        // source port 
        fprintf(fp, "%d : %d\t", tempfilt->filt.sp[0], tempfilt->filt.sp[1]);
        // destination port 
        fprintf(fp, "%d : %d\t", tempfilt->filt.dp[0], tempfilt->filt.dp[1]);
        // protocol 
        if (tempfilt->filt.prot_num == 0) {
            fprintf(fp, "0x00/0x00\t");
        } else {
            fprintf(fp, "0x%02x/0xFF\t", tempfilt->filt.prot_num);
        }
        // flags
        fprintf(fp, "0x%04x/0x%04x\t", tempfilt->filt.flags, tempfilt->filt.flags_mask);
        // ext_fields
        for (int j = 0; j < tempfilt->filt.num_ext_field; j++) {
            fprintf(fp, "%d\t", tempfilt->filt.ext_field[j]);
        } fprintf(fp, "\n");
    }
}

// return the number of filters stored in list
int FilterList::size() {
    return num;
}

