#pragma once

#include "intrusive_list.h"

namespace infinity {

template<int ID>
class PageCollection {
public:
    // Destructor
    ~PageCollection() {
        Clear();
    }

    // Returns the list_'s id
    int Id() const {
        return ID;
    }

    bool IsEmpty() const {
        return list_.IsEmpty();
    }

    size_t Size() const {
        return list_.Size();
    }

    // Returns the head
    Page *Head() const {
        return list_.Head();
    }

    // Returns the tail
    Page *Tail() const {
        return list_.Tail();
    }

    // Atomically applies the |visitor()| to each page
    template<typename Visitor>
    void ForEach(Visitor &visitor) {
        for (Page *p = Head(); p != 0; p = p->Next(ID)) {
            if (!visitor(p))
                break;
        }
    }

    // Atomically applies the |visitor()| to each page; starts at the tail
    template<typename Visitor>
    void ForEachReverse(Visitor &visitor) {
        for (Page *p = Tail(); p != 0; p = p->Previous(ID)) {
            if (!visitor(p))
                break;
        }
    }

    // Same as |for_each()|, but removes the page if |visitor()| returns true
    template<typename Visitor>
    void Extract(Visitor &visitor) {
        Page *page = Head();
        while (page) {
            Page *next = page->Next(ID);
            if (visitor(page)) {
                list_.Del(page);
            }
            page = next;
        }
    }

    // Clears the collection.
    void Clear() {
        Page *page = Head();
        while (page) {
            Page *next = page->Next(ID);
            list_.Del(page);
            page = next;
        }

    }

    // Returns a page from the collection; this is expensive!
    Page *Get(uint64_t address) const {
        for (Page *p = Head(); p != 0; p = p->Next(ID)) {
            if (p->Address() == address)
                return (p);
        }
        return 0;
    }

    // Removes a page from the collection. Returns true if the page was removed,
    // otherwise false (if the page was not in the list_)
    bool Del(Page *page) {
        if (Has(page)) {
            list_.Del(page);
            return true;
        }
        return false;
    }

    // Adds a new page at the head of the list_. Returns true if the page was
    // added, otherwise false (that's the case if the page is already part of
    // the list_)
    bool Put(Page *page) {
        if (!list_.Has(page)) {
            list_.Put(page);
            return true;
        }
        return false;
    }

    // Returns true if a page with the |address| is already stored.
    // This is expensive!
    bool Has(uint64_t address) const {
        return Get(address) != 0;
    }

    // Returns true if the |page| is already stored. This is much faster
    // than has(uint64_t address).
    bool Has(Page *page) const {
        return list_.Has(page);
    }

private:
    // The linked list
    IntrusiveList<Page, ID> list_;
};

}