// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   while (!empty()) {
      erase(begin());
   }
}

//
// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);
   if (empty()) {
      node* newNode = new node(anchor(), anchor(), pair);
      anchor_.next = newNode;
      anchor_.prev = newNode;
      return iterator(newNode);
   }
   iterator forward = anchor_.next;
   iterator reverse = anchor_.prev;
   //iterate the list from both ends. The point of intersect
   //is the location to insert the new node.
   while (forward != anchor()
       && reverse != anchor()
       && less(forward->first, reverse->first)) {
      if (less(forward->first, pair.first)) {
         ++forward;
      }
      if (less(pair.first, reverse->first)) {
         --reverse;
      }
   }
   if (forward == reverse) {
      //key is found, change the value.
      forward->second = pair.second;
      return forward;
   }
   //key is not in the list and will be inserted in front
   //of reverse iterator
   node* newNode = new node(reverse.where->next, reverse.where, pair);
   reverse.where->next = newNode;
   newNode->next->prev = newNode;
   return iterator(newNode);
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);
   if (empty()) {
      return end();
   }
   iterator forward = anchor_.next;
   iterator reverse = anchor_.prev;
   //iterate the list from both ends. The point of intersect
   //is the location of the desired node.
   while (forward != anchor()
       && reverse != anchor()
       && less(forward->first, reverse->first)) {
      if (less(forward->first, that)) {
         ++forward;
      }
      if (less(that, reverse->first)) {
         --reverse;
      }
   }
   if (forward == reverse) {
      //key is found.
      return forward;
   }
   return end();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   if (position == anchor()) {
      //can't delete anchor_
      return anchor();
   }
   position.where->next->prev = position.where->prev;
   position.where->prev->next = position.where->next;
   iterator temp{position.where->next};
   delete(position.where);
   return temp;
}


