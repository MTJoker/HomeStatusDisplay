#pragma once

/*
 * Pre-Allocated SingleLinkedList
 */
 
#include <stdint.h>
#include <stddef.h>

template<class T>
struct ListNode
{
	T* data;
	ListNode<T> *next;
};

template <class T>
class PreAllocatedLinkedList
{

public:

  /**
   * Create list with maxCount elements. List node memory as well
   * as data memory are maintained inside the list.
   */
	PreAllocatedLinkedList(uint32_t maxCount);
	~PreAllocatedLinkedList();

	/*
	 * Get number of elements in the list
	 */
	int size() const;
 
  /*
	 * Remove element at specified index. Returns
   * false if element does not exist.
	 */
	bool remove(int index);
 
	/*
	 * Get the element at index (for writing to it)
	 */
	const T* const get(int index) const;

  /*
   * Set element at index to data
   */
  bool set(int index, T& data);

	/*
	 * Clear the entire list. Return false if
   * nothing to clear (list empty).
	 */
	bool clear();

  /*
   * Check if list is full. If true, no adding
   * of further elements is possible.
   */
  bool isFull() const;
  
private:

  int m_size;
  ListNode<T>* m_root;
  uint32_t m_maxCount;

  ListNode<T>* getLast() const;
  ListNode<T>* getNode(int index) const;
};

template<typename T>
PreAllocatedLinkedList<T>::PreAllocatedLinkedList(uint32_t maxCount)
:
m_size(0),
m_root(NULL),
m_maxCount(maxCount)
{
  for(uint32_t i = 0; i < m_maxCount; i++)
  {
    ListNode<T> *tmp = new ListNode<T>();
    tmp->data = new T;

    // chain in, but do not increment size
    tmp->next = m_root;
    m_root = tmp;
  }
}

template<typename T>
PreAllocatedLinkedList<T>::~PreAllocatedLinkedList()
{
	ListNode<T>* tmp;
 
	while(m_root)
	{
		tmp = m_root;
		m_root = m_root->next;
    delete tmp->data;
		delete tmp;
	}
 
	m_size = 0;
}

template<typename T>
int PreAllocatedLinkedList<T>::size() const
{
  return m_size;
}

template<typename T>
bool PreAllocatedLinkedList<T>::remove(int index)
{
  bool ret = false;
  
  if(index < size())
  {
    ListNode<T>* previous = NULL;
    ListNode<T>* current = NULL;
    ListNode<T>* last = getLast();

    // unchain element from list and add it at the end again
    if(index == 0)
    {
      current = m_root;
      m_root = current->next;
    }
    else
    {
      previous = getNode(index - 1);
      current = previous->next;
      previous->next =current->next;
    }

    ListNode<T>* next = last->next;
    last->next = current;
    current->next = next;
    m_size--;
    ret = true;
  }

	return ret;
}

template<typename T>
const T* const PreAllocatedLinkedList<T>::get(int index) const
{
  ListNode<T>* tmp = NULL;

  if(m_size == 0)
  {
    // no entries added up to now
    tmp = m_root;
  }
  else if(index < m_size)
  {
    // get existing node
    tmp = getNode(index);
  }
  else if(!isFull())
  {
    // use new node if possible
    tmp = getNode(m_size);
  }
  
	return tmp? tmp->data : NULL;
}

template<typename T>
bool PreAllocatedLinkedList<T>::set(int index, T& entry)
{
  bool ret = false;
  
  T* data = const_cast<T*>(get(index));

  if(data)
  {
    *data = entry;

    if(index >= m_size)
    {
      // new element was added
      m_size++;
    }

    ret = true;
  }

  return ret;
}

template<typename T>
bool PreAllocatedLinkedList<T>::clear()
{
  bool ret = false;

  if(m_size > 0)
  {
    m_size = 0;
    ret = true;
  }

  return ret;
}

template<typename T>
bool PreAllocatedLinkedList<T>::isFull() const
{
  return (m_size >= m_maxCount) ? true : false;
}

template<typename T>
ListNode<T>* PreAllocatedLinkedList<T>::getNode(int index) const
{
  int pos = 0;
  ListNode<T>* current = m_root;

  while(pos < index && current)
  {
    current = current->next;
    pos++;
  }

  // Check if the object index got is the same as the required
  if(pos == index)
  {
    return current;
  }

  return NULL;
}

template<typename T>
ListNode<T>* PreAllocatedLinkedList<T>::getLast() const
{
  ListNode<T>* tmp = m_root;

  if(m_size > 0)
  {
     for(uint32_t i = 0; i < m_size - 1; i++)
     {
        tmp = tmp->next;
     }
  }

  return tmp;
}

