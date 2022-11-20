
#include "ExtendiableHashing.h"
#include <bitset>
#include <iostream>
#include <cmath>
#include <string>
using namespace std;


void displayNotFound(int key) {
	std::cout << "\t\t\t key:  " << key << " \t value : \t not found" << std::endl;
}

//Display one record entry, please don't change this function
void displayItem(DataItem* dataItem) {
	if (dataItem != 0 && dataItem->valid)
	{
		std::cout << "\t\t\t key: " << std::bitset<8>(dataItem->key) << "\t value:\t" << dataItem->data << std::endl;
	}
	else {
		std::cout << "\t\t\t key:  ============ \t value:\t empty data" << std::endl;
	}
}

void displayBucket(Bucket& currentBucket, string& depths, string& values, int verbose) {
	depths.append(std::to_string(currentBucket.localDepth));
	depths.append(",");
	values.append("[");
	if (verbose)
		std::cout << "\t\tBucket:\t local depth:" << currentBucket.localDepth << std::endl;
	if (currentBucket.currentEntries == 0)
	{
		if (verbose)
			std::cout << "\t\t\tNo Data yet\n";
		for (int i = 0; i < RECORDSPERBUCKET; i++)
		{
			values.append("null");
			values.append(",");
		}
	}
	else {
		for (int i = 0; i < RECORDSPERBUCKET; i++)
		{
			if (currentBucket.dataItem[i].valid)
			{
				values.append(std::to_string(currentBucket.dataItem[i].data));
				values.append(",");
				if (verbose)
					std::cout << "\t\t\t key: " << std::bitset<8>(currentBucket.dataItem[i].key) << "\t value:\t" << currentBucket.dataItem[i].data << std::endl;
			}
			else {
				values.append("null");
				values.append(",");
				if (verbose)
					std::cout << "\t\t\t key:  ============ \t value:\t empty data" << std::endl;
			}

		}
	}
	values.pop_back();
	values.append("]");

}

void displayDirectory(GlobalDirectory& globaldirectory, Bucket& currentBucket, int verbose) {
	std::cout << "Directory:\t global depth:" << globaldirectory.globalDepth << std::endl;
	string values = "(";
	string depths = "(";
	int count = 0;
	//string locations = "(";
	if (globaldirectory.length == 0)
	{
		count++;
		std::cout << "\tNo Directory yet\n";
		displayBucket(currentBucket, depths, values, verbose);
	}
	else {

		for (int i = 0; i < globaldirectory.length; i++)
		{
			if (i == 0)
			{
				count++;
			}
			else {
				if (globaldirectory.entry[i - 1] != globaldirectory.entry[i])
					count++;
			}
			if (verbose)
				std::cout << "\t key: " << std::bitset<8>(i) << "\t value:\t" << globaldirectory.entry[i] << std::endl;
			displayBucket(*globaldirectory.entry[i], depths, values, verbose);
			if (verbose)
				std::cout << "-----------------------------------------------\n\n";
		}
		//values.pop_back();
		depths.pop_back();

	}

	values.append(")");
	depths.append(")");
	std::cout << " buckets:\t" << count << "/" << globaldirectory.length << endl;
	std::cout << "values:\t" << values << endl;
	std::cout << "depths:\t" << depths << endl;
	std::cout << "=========================\n";
	//std::cout << "Press any key to continue\n";
	char t[100];
	//std::cin >> t;


}



//Hashing function and getting directory Index, please don't change this function
int getCurrentHash(int key, int depth) {
	int hashedKey = (key & MAXKEYVALUE) >> (MAXKEYLENGTH - depth);
	return hashedKey;
}

//TODO1: Implement this function, Don't change the interface please
// functionlity: try to insert item into a bucket
// return:   1 if succedded
//			 0 if failed
// input:   currentBucket, dataItem to be inserted (check ExtendiableHashing.h for the content of the file)
// Hint1: don't forget to update currentEntries upon insertion, you will need it later

int insertItemIntoBucket(Bucket& currentBucket, DataItem data)
{
	int i = 0;
	while ( i < RECORDSPERBUCKET )
	{
		if (currentBucket.dataItem[i].valid == 0) {
			currentBucket.dataItem[i] = data;
			currentBucket.dataItem[i].valid = 1;
			++currentBucket.currentEntries;
			return 1;
		}
		i++;

	}
	return 0;
}

//TODO2: Implement this function, Don't change the interface please
// functionlity: look for an item in a bucket using key, if found call displayItem(..), if not found call displayNotFound()
// return:   nothing
// input:   currentBucket, key value to search for
// Hint: use displayNotFound & displayItem functions

void findItemInBucket(Bucket& currentBucket, int key)
{
	for (auto& item : currentBucket.dataItem) {
		if (item.key == key) {
			displayItem(&item);
			return;
		}
	}
	displayNotFound(key);
}


//TODO3: Implement this function, Don't change the interface please
// functionlity: try to Delete item based on a key value from a bucket
// return:   1 if succedded
//			 0 if failed (when does it fail to delete??)
// input:   currentBucket, key to be inserted (check ExtendiableHashing.h)
// Hint1:   don't forget to update currentEntries upon deletion, you will need it later

int deleteItemFromBucket(Bucket& currentBucket, int key)
{
	int i = 0;
	while (i < RECORDSPERBUCKET)
	{
		if (currentBucket.dataItem[i].key == key) {
			currentBucket.dataItem[i].valid = 0;
			--currentBucket.currentEntries;
			return 1;
		}
		i++;

	}
	return 0;
}


//TODO4: Implement this function, Don't change the interface please
// functionlity: try to insert item in the file, if the bucket is full, extend the directory,
//				  if you extended the directory five times but it still doesn't work, return 0 
// return:   1 if succedded
//			 0 if failed (when does it fail to insert??)
// input:   data:--> dataitem to be inserted, currentBucket --> initialBucket before creating director , globaldirectory
// Hint: use insertItemIntoBucket,getCurrentHash, extendDirectory functions
// Hint1:   don't forget to check for corner cases, for example if several entries points to the same bucket and you are going to split it
// Hint2:   a bucket could be split without expanding the directory (think when this will happen?)
// Hint3:   don't forget to delete extra data

int insertItem(DataItem data, Bucket& currentBucket, GlobalDirectory& globaldirectory)
{
	//First corner case is done for you: if no directory yet
	if (globaldirectory.globalDepth == 0) {
		if (insertItemIntoBucket(currentBucket, data) == 0) {
			createFirstTimeDirectory(globaldirectory, currentBucket);
		}
		else {
			return 1; //successfully inserted;
		}
	}

	// complete the rest of the function ....

	for (int i = 0; i < 5; ++i) {
		int dataHash = getCurrentHash(data.key, globaldirectory.globalDepth);
		int isInserted = insertItemIntoBucket(*globaldirectory.entry[dataHash], data);
		if (isInserted) return 1;

		if (globaldirectory.globalDepth == globaldirectory.entry[dataHash]->localDepth) {
			extendDirectory(globaldirectory, dataHash);

		} else if (globaldirectory.globalDepth > globaldirectory.entry[dataHash]->localDepth) {
			Bucket* newBucket1 = new Bucket(globaldirectory.entry[dataHash]->localDepth + 1);
			Bucket* newBucket2 = new Bucket(globaldirectory.entry[dataHash]->localDepth + 1);
			Bucket* oldBucket = globaldirectory.entry[dataHash];
			int currentHash = getCurrentHash(data.key, globaldirectory.entry[dataHash]->localDepth);
			currentHash <<= globaldirectory.globalDepth - globaldirectory.entry[dataHash]->localDepth;

			int hash = currentHash;
			int iterations = pow(2, globaldirectory.globalDepth - globaldirectory.entry[dataHash]->localDepth);
			for (int j = 0; j < iterations / 2; ++j) {
				globaldirectory.entry[hash] = newBucket1;
				++hash;
			}

			for (int j = 0; j < iterations / 2; ++j) {
				globaldirectory.entry[hash] = newBucket2;
				++hash;
			}

			// ++globaldirectory.entry[dataHash]->localDepth;

			for (int j = 0; j < oldBucket->currentEntries; ++j) {
				DataItem currentItem = oldBucket->dataItem[j];
				int newHash = getCurrentHash(currentItem.key, globaldirectory.globalDepth);
				insertItemIntoBucket(*globaldirectory.entry[newHash], currentItem);
			}
			delete oldBucket;
		}
	}
	return 0;

}

//TODO5: Implement this function, Don't change the interface please
// functionlity: search the directory for an item using the key
// return:   nothing
// input:   key to be searched for , currentBucket --> initialBucket before creating directory,  globaldirectory
// Hint1:   use findItemInBucket & getCurrentHash functions

void searchItem(int key, Bucket& currentBucket, GlobalDirectory& globaldirectory) {
	if (globaldirectory.globalDepth == 0) {
		findItemInBucket(currentBucket, key);
	}
	else {
		int hash = getCurrentHash(key, globaldirectory.globalDepth);
		findItemInBucket(*globaldirectory.entry[hash], key);
		
	}
}

//TODO6: Implement this function, Don't change the interface please
// functionlity: search on an item based on the key and delete it.
// return:   1 if succedded
//			 0 if failed (when does it fail to delete??)
// input:   key to be searched for , currentBucket --> initialBucket before creating directory,  globaldirectory
// Hint:    use deleteItemFromBucket & getCurrentHash & checkDirectoryMinimization functions
// Hint1:   in case the whole bucket is empty, the bucket should be merged again and the pointer should point to the peer bucket
// Hint2:   in how many steps do we reach our goal?
// Hint3:   in case of delete success don't forget to call checkDirectoryMinimization to compress directory if needed.
// Hint4: You might want to loop on checkDirectoryMinimization, not just call it once to continue merging

int deleteItem(int key, Bucket& currentBucket, GlobalDirectory& globaldirectory) {
	int g_depth = globaldirectory.globalDepth;
	if (globaldirectory.globalDepth == 0)
	{
		return deleteItemFromBucket(currentBucket, key);
	}
	else
	{
		int Bucket_ = getCurrentHash(key, globaldirectory.globalDepth);

		return deleteItemFromBucket(*globaldirectory.entry[Bucket_], key);
	}

}





//create  the first directory, this might help you to implement extendDirectory
int createFirstTimeDirectory(GlobalDirectory& globaldirectory, Bucket& currentBucket)
{
	globaldirectory.globalDepth = 1;
	globaldirectory.length = 2;
	globaldirectory.entry = new Bucket * [globaldirectory.length];
	globaldirectory.entry[0] = new Bucket(globaldirectory.globalDepth);
	globaldirectory.entry[1] = new Bucket(globaldirectory.globalDepth);
	for (int i = 0; i < RECORDSPERBUCKET; i++) {
		int newKey = getCurrentHash(currentBucket.dataItem[i].key, globaldirectory.globalDepth);
		if (newKey == -1)
		{
			return -1;
		}
		insertItemIntoBucket(*globaldirectory.entry[newKey], currentBucket.dataItem[i]);
	}
	return 1;
}


//TODO7: Implement this function, Don't change the interface please
// functionlity: this expands the directory because we can't find a space anymore in the file,
//               it also redistrubtes data over the split buckets
// return:   1 if succedded
//			 0 if failed (when does it fail to expand??)
// input:   Directory, hashKey(bucket index) at which the overflow occurs
// Hint1:   don't forget todelete unneeded pointers to avoid memory leakage
// Hint2:   what is the size of the new directory compared to old one? what is the new depth?
// Hint3:   some entries will point to the same bucket
int extendDirectory(GlobalDirectory& globaldirectory, int splitIndex) {
	if (globaldirectory.globalDepth >= 8) return 0;
	Bucket** old_Buckets = globaldirectory.entry;

	++globaldirectory.globalDepth;

	Bucket* old_entry = old_Buckets[getCurrentHash(splitIndex, globaldirectory.globalDepth)];

	globaldirectory.length *= 2;

	globaldirectory.entry = new Bucket * [globaldirectory.length];

	for (int i = 0; i < globaldirectory.length / 2; ++i) {
		if (i != getCurrentHash(splitIndex, globaldirectory.globalDepth)) {
			globaldirectory.entry[2 * i] = old_Buckets[i];
			globaldirectory.entry[2 * i + 1] = old_Buckets[i];	

		} else {
			globaldirectory.entry[2 * i] = new Bucket(globaldirectory.globalDepth);
			globaldirectory.entry[2 * i + 1] = new Bucket(globaldirectory.globalDepth);

			for (int j = 0; j < RECORDSPERBUCKET; j++) {
				int entry_index = getCurrentHash(old_entry->dataItem[j].key, globaldirectory.globalDepth);
				insertItem(old_entry->dataItem[j], *globaldirectory.entry[entry_index], globaldirectory);
			}
		}
	}
	delete[] old_Buckets;
	return 1;
}



//If all buckets have depth less than global depth,
// compress the directory depth by one 
// loop on this function as appropriate to compress the file back
int checkDirectoryMinimization(GlobalDirectory& globaldirectory) {

	for (int i = 0; i < globaldirectory.length; i++) {
		if (globaldirectory.entry[i]->localDepth == globaldirectory.globalDepth)
			return -1;

	}
	//if its depth =1, there is nothing more we can do, stop here
	if (globaldirectory.length == 1)
		return -1;
	//all are smaller than localdepth, initiate minimization
	// this is actually too stupid, it should be at least half empty to initiate minimization
	// but we will keep it like that for simplicity
	int oldGlobalDepth, oldLength;
	Bucket** oldEntry;

	oldGlobalDepth = globaldirectory.globalDepth;
	oldLength = globaldirectory.length;
	globaldirectory.globalDepth -= 1;
	globaldirectory.length /= 2;
	oldEntry = globaldirectory.entry;
	globaldirectory.entry = new Bucket * [globaldirectory.length];
	for (int i = 0; i < globaldirectory.length; i++) {
		globaldirectory.entry[i] = oldEntry[2 * i];
	}

	delete[] oldEntry;
	//delete oldEntry;
	return 1;

}
