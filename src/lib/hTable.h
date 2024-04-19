
struct hTableItem{
    char* key;
    void* value;
    struct hTableItem* next;
};

//A hash table storing keys and void* as values. For collision handling a linked list approach has been chosen
typedef struct hashTable{
    struct hTableItem** items;
    size_t size; //the size of the array
    int count; //how many elements we have in the array
} hashTable;

//Macro wrapper for two for loops that provides a struct hTableItem* item variable for your use
#define forHashTableItem(table) \
    for(int tableIndex=0; tableIndex < table->size; tableIndex++)\
    for(struct hTableItem* item = table->items[tableIndex]; item != NULL; item=item->next)

//Initializes a hash table with a given size preallocated and set to NULL
hashTable* initHashTable(size_t size);

//Frees all the allocated memory in the hash table including the items, but not the values of the items.
void freeHashTable(hashTable* table);

//Inserts the given void* as the value of the given key in the given hashTable. 
int insertHashItem(hashTable* table, const char* key, const void* value);

//Retrieves the value from the hashTable. Be sure to cast the returned pointer
void* getVal(hashTable* table, const char* key);

//Returns an array of all value pointers in the hash table
void** hashTableToArray(hashTable* table);