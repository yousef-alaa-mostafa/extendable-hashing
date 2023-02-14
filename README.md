# Extendable Hashing
This C++ project implements Extendable Hashing, a dynamic hash table that grows as data is added.

## Features
- Insert, search, and delete items
- Display the directory, current hash, and buckets
- Create the initial directory
- Extend the directory when needed
- Check for directory minimization

## Getting Started
To run the project:
1. Clone the repository.
2. Open the project in your preferred C++ IDE.
3. Build and run the project.

## Usage
Use the following functions to interact with the hash table:
<br />
- insertItemIntoBucket(Bucket& currentBucket, DataItem data) - Inserts an item into the hash table
- findItemInBucket(currentBucket, key) - Searches for an item with the given key
- deleteItemFromBucket(currentBucket, key) - Deletes an item with the given key
- displayDirectory() - Displays the current directory
- displayBucket(hash) - Displays the bucket with the given hash
- displayItem(dataItem) - Displays the item at the given index in the bucket with the given hash
- getCurrentHash(key, depth) - Returns the hash for the given key
- createFirstTimeDirectory() - Creates the initial directory
- extendDirectory() - Extends the directory when needed
- checkDirectoryMinimization() - Checks if directory minimization is possible

## Example
To demonstrate how Extendable Hashing works, we have included hand calculations in the example file(Hand calculations.pdf).

## Contributing
We welcome contributions to this project! If you find a bug or would like to suggest an improvement, please open an issue or pull request.

## License
This project is licensed under the MIT License.
