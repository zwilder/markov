## Markov Chain Word Generation

Check [the blog](https://zwilder.github.io/posts/2023-11-17-markov/) for a more
detailed description of this project. 

Build with 'make optimized'.

```
Usage:
    markov [-l] [-n number] [-o outfile] infile1 [infile2...]
    markov -g infile1 -s infile2 [-l -f] [-n number] [-o outfile]
Where:
    infile1 [infile2...] are data files containing space separated words
    [-l] writes a log file to "log.txt" in the current directory
    [-n number] is number of names to generate
    [-o outfile] is the file to write the output to
    -g infile1 -s infile2 are input data for a "Genre species" output
    [-f] when used with -g -s, prints output as a "First Last" word.
Example: "markov -n 100 data1.txt data2.txt" will generate 100 random names
 using data1.txt and data2.txt as input.
Example: "markov -n 100 -o out.txt -g data1.txt -s data2.txt" will generate 100
random "Genre species" style names, and write them to "out.txt"
```
 
--

Markov chain algorithm basically goes like:

1: Parse the list and generate a hash table, where each key is a pair of
letters, and the item is a list of letters that follows that pair.

Example: 

> Robert James John William Richard Charles Donald David Thomas George Ronald

Would generate a table would look like:

``` 
{
 ['ro',['b','n']]
 ['ob',['e']]
 ['be',['r']]
 ['er',['t']]
 ['rt',[' ']]
 ['ja',['m']]
 ['am',['e']]
 ['me',['s']]
 ['es',[' ',' ']]
 ['jo',['h']]
 ['oh',['n']]
 ['hn',[' ']]
 ['wi',['l']]
 ['il',['l']]
 ['ll',['i']]
 ['li',['a']]
 ['ia',['m']]
 ['am',[' ']]
 ['ri',['c']]
 ['ic',['h']]
 ['ch',['a','a']]
 ['ha',['r','r']]
 ['ar',['d','l']]
 ['rl',['e']]
 ['le',['s']]
 ['do',['n']]
 ['on',['a','a']]
 ...
}
```

2: Choose a random item from the list to start the name.
3: Construct the name by:
 - Taking two letters and searching the dataset (a1a2)
 - Choose next letter at random from dataset letters (a1a2a3)
 - Take next two letters (a2a3) and search dataset
 - Choose next letter at random from dataset letters (a1a2a3a4)
 - Repeat until a ' ' is chosen or no match is found
 
Examples:
- Choose an item at random: Ro
- Next letter is: b ('Rob')
- ob -> ['e'] 'Robe'
- be -> ['r'] 'Rober'
- er -> ['t'] 'Robert'
- rt -> [' '] -> Result is "Robert"

- Random item: Ri
- Ri -> ['c'] 'Ric'
- ic -> ['h'] 'Rich'
- ch -> ['a','a'] 'Richa'
- ha -> ['r','l'] 'Richarl'
- rl -> ['e'] 'Richarle'
- le -> ['s'] 'Richarles'
- es -> [' ',' '] -> Result is "Richarles"

Obviously, larger dataset gives more interesting results, and I don't want to
generate that dataset by hand.

The important thing here is the "duplicate results. If a letter pair key had
a value like this:
- ["ar",['e', 'e', 'i', 'l', 'o', 'e', 'j', 'e']]

Then "ar" would have a:
- 4/8 chance of being followed by 'e'
- 1/8 chance of being followed by 'i'
- 1/8 chance of being followed by 'l'
- 1/8 chance of being followed by 'o'
- 1/8 chance of being followed by 'j'

