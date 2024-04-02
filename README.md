# logfind
This is a windows application version of grep made in pure C, created from the book Learn C Hard Way, that only shows if the word exists in the file inside the folder or relatively to you cmd path

## This programs need make to compile
- $ git clone the repostory
- $ make

## App Usage
this programs accepts any parameters do you want the default is and logical
- $ ./logfind "word" ... /// ("and" logical it will find if all words is inside the archive)
- $ ./logfind -o "word" ... /// ("or" logical it will find every word independentily)
