//Hashtable.h for Operating Systems class - a basis for all future programs in order to manipulate a Concurrent Hashtable


#pragma once

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

//vector of vectors, once you get into the vector you are looking for, lock first then unlock

/// ConcurrentHashTable is a concurrent hash table (a Key/Value store).  It is
/// not resizable, which means that the O(1) guarantees of a hash table are lost
/// if the number of elements in the table gets too big.
///
/// The ConcurrentHashTable is templated on the Key and Value types
///
/// The general structure of the ConcurrentHashTable is that we have an array of
/// buckets.  Each bucket has a mutex and a vector of entries.  Each entry is a
/// pair, consisting of a key and a value.  We can use std::hash() to choose a
/// bucket from a key.
template <typename K, typename V>
class ConcurrentHashTable
{

  struct Entry
  {
    K key;
    V value;
  };

  vector<struct Entry> *buckets;
  std::mutex *mtxs;

public:
  std::mutex mtx;
  int numBuckets;

  /// Construct a concurrent hash table by specifying the number of buckets it
  /// should have
  ///
  /// @param _buckets The number of buckets in the concurrent hash table
  ConcurrentHashTable(size_t _buckets)
  {
    if (_buckets < 1)
    {
      cout << "Error. Size of bukets should be more than 0."
           << "\n";
    }
    this->numBuckets = _buckets;
    buckets = new vector<struct Entry>[_buckets];
    mtxs = new mutex[_buckets];
  }

  /// Clear the Concurrent Hash Table.  This operation needs to use 2pl
  void clear()
  {
    for (int i = 0; i < numBuckets; i++)
    {
      mtxs[i].lock();
    }

    for (auto &bucket : buckets)
    {
      bucket.clear();
    }

    for (int i = 0; i < numBuckets; i++)
    {
      mtxs[i].unlock();
    }
  }

  /// Insert the provided key/value pair only if there is no mapping for the key
  /// yet.
  ///
  /// @param key The key to insert
  /// @param val The value to insert
  ///
  /// @returns true if the key/value was inserted, false if the key already
  /// existed in the table
  bool insert(K key, V val)
  {
    int hash_val = std::hash<K>()(key) % numBuckets;
    mtxs[hash_val].lock();

    for (auto i = buckets[hash_val].begin(); i != buckets[hash_val].end(); i++)
    {
      //trying to compare the hash values at the specific key
      if (i->key == key)
      {
        mtxs[hash_val].unlock();
        //if the hash value exists in a bucket already
        return false; //do not reinsert
      }
    }
    buckets[hash_val].push_back({key, val});

    mtxs[hash_val].unlock();
    return true; // depends if it was successful
  }

  /// Insert the provided key/value pair if there is no mapping for the key yet.
  /// If there is a key, then update the mapping by replacing the old value with
  /// the provided value
  ///
  /// @param key The key to upsert
  /// @param val The value to upsert
  ///
  /// @returns true if the key/value was inserted, false if the key already
  ///          existed in the table and was thus updated instead
  bool upsert(K key, V val)
  {
    int hash_val = std::hash<K>()(key) % size;
    mtxs[hash_val].lock();

    for (auto i = buckets[hash_val].begin(); i != buckets[hash_val].end(); i++)
    {
      //trying to compare the hash values at the specific key
      if (i->key == key)
      {
        i->value = val;
        mtxs[hash_val].unlock();
        //if the hash value exists in a bucket already
        return false; //do not reinsert
      }
    }
    buckets[hash_val].push_back({key, val});

    mtxs[hash_val].unlock();
    return true; // depends if it was successful
  }

  /// Apply a function to the value associated with a given key.  The function
  /// is allowed to modify the value.
  ///
  /// @param key The key whose value will be modified
  /// @param f   The function to apply to the key's value
  ///
  /// @returns true if the key existed and the function was applied, false
  ///          otherwise
  bool do_with(K key, std::function<void(V &)> f)
  {
    int hash_val = std::hash<K>()(key) % numBuckets;
    for (auto i = buckets[hash_val].begin(); i != buckets[hash_val].end(); i++)
    {
      if (i->key == key)
      {
        f(i->v);
        return true;
      }
    }
    return false;
  }

  /// Apply a function to the value associated with a given key.  The function
  /// is not allowed to modify the value.
  ///
  /// @param key The key whose value will be modified
  /// @param f   The function to apply to the key's value
  ///
  /// @returns true if the key existed and the function was applied, false
  ///          otherwise
  bool do_with_readonly(K key, std::function<void(const V &)> f)
  {
    int hash_val = std::hash<K>()(key) % numBuckets;
    for (auto i = buckets[hash_val].begin(); i != buckets[hash_val].end(); i++)
    {
      if (i->key == key)
      {
        f(i->value);
        return true;
      }
    }
    return false;
  }

  /// Remove the mapping from a key to its value
  ///
  /// @param key The key whose mapping should be removed
  ///
  /// @returns true if the key was found and the value unmapped, false otherwise
  bool remove(K key)
  {
    int hash_val = std::hash<K>()(key) % numBuckets;
    mtxs[hash_val].lock();

    for (auto i = buckets[hash_val].begin(); i != buckets[hash_val].end(); i++)
    {
      //trying to compare the hash values at the specific key
      if (i->key == key)
      {
        buckets[hash_val].erase(i);
        mtxs[hash_val].unlock();
        return true; //do not reinsert
      }
    }

    mtxs[hash_val].unlock();
    return false;
  }

  /// Apply a function to every key/value pair in the ConcurrentHashTable.  Note
  /// that the function is not allowed to modify keys or values.
  ///
  /// @param f    The function to apply to each key/value pair
  /// @param then A function to run when this is done, but before unlocking...
  ///             useful for 2pl
  void do_all_readonly(std::function<void(const K, const V &)> f, std::function<void()> then)
  {
    //get all locks
    for (int i = 0; i < numBuckets; i++)
    {
      mtxs[i].lock();
    }

    for (int i = 0; i < numBuckets; i++)
    {
      // loop through the buckets :  for (auto &bucket : buckets)
      for (auto j = buckets[i].begin(); j < buckets[i].end(); j++)
      {
        f(j->K, j->V);
      }
    }
    //function to run when this is done
    then();

    // make sure to RELEASE ALL THE LOCKS
    for (int i = 0; i < numBuckets; i++)
    {
      mtxs[i].unlock();
    }
  }
};
