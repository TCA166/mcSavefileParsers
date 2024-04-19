/*!
 @file hTable.h
 @brief A hash table implementation
*/

/*!
 @defgroup hTable Hash Table
 @brief A hash table implementation
*/

/*!
 @struct hTableItem
 @brief A hash table item structure
 @ingroup hTable
*/
struct hTableItem{
    /*!
     @var key
     @brief the item key, used to identify the value in hash collisions
    */
    char* key;
    /*!
     @var value
     @brief whatever value the key is holding
    */
    void* value;
    /*!
     @var next
     @brief the next item in the linked list
     @details can be NULL
    */
    struct hTableItem* next;
};

/*!
 @struct hashTable
 @brief A hash table storing keys and void* as values. For collision handling a linked list approach has been chosen
 @ingroup hTable
*/
typedef struct{
    /*!
     @var items
     @brief Nullable array of hash table items
     @details Key hashes point into this array, and if the value under the hash isn't null then the item is inserted at the end of the underlying linked list
    */
    struct hTableItem** items;
    /*!
     @var size
     @brief The size of the array, not meant to be changed
    */
    size_t size; //the size of the array
    /*!
     @var count
     @brief how many elements we have in the array
    */
    int count;
} hashTable;

/*!
 @def forHashTableItem(key, table)
 @brief Macro wrapper for two for loops that provides a struct hTableItem* item variable for your use
 @param table The hash table to get the index for
 @ingroup hTable
*/
#define forHashTableItem(table) \
    for(int tableIndex=0; tableIndex < table->size; tableIndex++)\
    for(struct hTableItem* item = table->items[tableIndex]; item != NULL; item=item->next)

/*!
 @brief Initializes a hash table with a given size preallocated and set to NULL
 @param size The size of the hash table
 @return The initialized hash table
 @ingroup hTable
*/
hashTable* initHashTable(size_t size);

/*!
 @brief Frees all the allocated memory in the hash table including the items, but not the values of the items.
 @param table The hash table to free
 @ingroup hTable
*/
void freeHashTable(hashTable* table);

/*!
 @brief Inserts the given void* as the value of the given key in the given hashTable. 
 @details If the key already exists in the hashTable, the value is updated.
 @param table The hashTable to insert the value into
 @param key The key to insert the value under
 @param value The value to insert
 @return The number of elements in the hashTable
 @ingroup hTable
*/
int insertHashItem(hashTable* table, const char* key, const void* value);

/*!
 @brief Retrieves the value from the hashTable. Be sure to cast the returned pointer
 @param table The hashTable to retrieve the value from
 @param key The key to retrieve the value from
 @return The value pointer or NULL if the key doesn't exist
 @ingroup hTable
*/
void* getVal(hashTable* table, const char* key);

/*!
 @brief Returns an array of all value pointers in the hash table
 @param table The hash table to convert to an array
 @return The array of value pointers
 @ingroup hTable
*/
void** hashTableToArray(hashTable* table);