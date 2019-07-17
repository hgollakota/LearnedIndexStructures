cALLiX - c Adaptive Linear Learned indeX

Current Writeup -- 7/17/19

Generated a million+-key normal distribution with mean 0 and stddeviation 100. Implemented a print(outfile) class which dumps "header" information about each gapped array as well as all the keys, positions, and bits for each entry in the array.

printheader() on the other hand does the same thing but only prints the "header" information, which include the linear model for each gapped array, whether the array is in sorted order, its minimum and maximum keys, and the bitset (read from bottom to top!) I've been using printheader() once I determined that the arrays seem to consistently be in sorted order. The clusterwhoops of code that I've been using to find the appropriate location and make a gap is working as intended.

For the million+-key example above, with a GA size of 4096 and a max density of 0.5, each array should contain ~1000 - 2000 keys. This would mean that the number of GAs (or leaves in the overall model) should be between 500-1000. The test resulted in 761. 

I have implemented lookup(key), which according to the original paper is where a cALLiX should really shine with regards to speed. I still need to test lookup.

I want to generate a nice visual graph of a non-standard cumulative distribution function, and show how the linear models of each cALLiX when bounded by its min-max keys create an approximation of the CDF.

I need to learn more about static or constant or global variables in c++ - at first I want to be able to change the size of a GappedArray to different sizes than 4096, as well as change the maximum density from 0.5, as well as (might not be as crucial but could save time) change how many samples are collected from the GappedArray during expansion. 



7/16/19
Running tests on insert prior to reimplementing expand.
Running main, generates output1.txt that identifies some of the keys that are out of order. Need to figure out why this is occurring. 
Tests inserting at beginning and ends of arrays seem to be working.

I think part of the problem is I'm getting lost in all the IF..ELSE statements that could be a LOT cleaner

insert is working! I needed to change binary search to return the lowerbound as opposed to the midpoint.
on to expand()

EXPAND IS WORKING!!! I'm so excited. uncommented code in main right now generates new data to test with.

7/9/19

Implemented so Far:

Linear Model class (y = ax + b), returns and allows for changes to a and b and will return a y when given an x. Also, when given a vector of keys and a vector of indices, will construct a least-squares regression line O(2N) LinearModel.

MinMaxTransformer class, will return a y that is scaled based on an old minimum - old maximum as a value either [0,1] or to a new minimum and maximum.

To Implement:

Gapped Array:

I was largely focused on following along with what the ALEX paper does, with a few additions. The Gapped Array is structured as a binary tree, where only the leaf nodes contain the arrays while all internal nodes contain the linear model that determines which leaf to assign a key to or return a key from. The root node contains the initial constructor (so a "cold-start") as well as methods to collect diagnostic information from all its children nodes.

GappedArray::Insert:

If the inserted key increases the density beyond the maximum density of 0.5 (tunable) then the array needs to be split into two child gapped arrays (see GappedArray::Expand).

Otherwise, the original predicted index of where to insert is based on the linear model. If the index is empty, and the next occupied index is a higher key and the previous occupied index is a lower key, then we're done with the insert.

If the predicted index violates any of those conditions, than an exponential search is conducted to find the actual proper position to insert the key, starting with the predicted index. This will result in either finding an unoccupied position, or an occupied position.

If unoccupied, store the key and we're done. If occupied, need to shift keys in the direction of the nearest gap so that the space becomes unoccupied, then store the key in that location.

If the key happens to be lower than the lowest minimum, or higher than the highest maximum, this is fine. We just shift all the occupied keys from that edge up or down respectively and store this key as the new minimum or maximum. This accounts for any shortcomings of the model, and after enough keys are added to the ends and the density goes high enough, the GappedArray::Expand operation should account for this shift. If these keys however are just outliers, ideally the Least Squares Regression should be robust enough to handle this.

GappedArray::Expand

This is where the Learned part comes into play. When it is determined that the array has reached its maximum density, then we locate the median key/index pair. We construct 4 vectors using sampling (not random sampling, but stratified like every 100th value, minimum and maximum inclusive). Two vectors (left child) will be the keys and indexes those keys occupy array[0] up to the array[median-1]. The other two vectors will be the keys and occupied indexes of median to array[array.size()-1].

Then we apply  a MinMaxTransformation to just the indexes, to fit the new child gapped arrays. This is important because it allows us to account for skew, and is mathematically permissible because it's just scaling (linear transformation) in the end.

Then we construct two new LinearModels for each half of the parent array, based on the keys and transformed y values.

Using these two new models, we assign all the values from each half of the parent gapped-array to the new children gapped arrays, then delete the parent gapped array.

At this point, any future insert/lookup/delete operations will run through the parent's linear model, which will determine which of the two child linear models the key should next run through, which will eventually determine which index the key should be stored in.

GappedArray::Lookup

Key -> root linearModel -> internal1 linear model -> internal2 linear model -> ... -> leaf linear model -> Array. Find the predicted spot for the key. If it is empty and is in the proper position, then we know the key does not exist. Otherwise, exponential search to find the position where the key is actually supposed to be to determine if the key exists there or not.

GappedArray::Delete

This is the part that goes beyond the original paper. My idea is that as keys are deleted, we do a quick comparison of the density between the two sibling nodes. If the siblings combined are a lower density than a threshold (say .15) then we calculate the ratio between the two densities, which determines how many key/index pairs to sample from each child array. We sample these pairs, do a min max transform again, determine a new linear model for the parent node. We then store all the keys from the child node into a new array stored in the parent, and afterwards delete the child nodes.

GappedArray::Compression

Another potential difference, in that perhaps we can collapse some of the internal nodes, and combine linear models into cases divided by key boundaries. This allows us to reduce the number of calculations needed. This is not what I'm originally planning on implementing but could be useful for the future.

Tuneable Parameters:

Size of each Array: Smaller arrays allow for quicker adaptation to the distribution, while larger arrays reduce the number of expensive Expand operations. Maybe this can be changed based on the key distribution, as in if the key distribution is largely stable then the child arrays can safely become larger and larger with each generation.

Density: Obvious tuneable parameter, with tradeoff between size and speed. Also, larger more empty arrays will have less collisions, but could also be poorer approximations of

Number of children to split into: This one is interesting, because it goes into 50+ years of work in approximating a curve using lines. There are probably better algorithms than simply splitting an array into two children. Maybe initially the array can be split into 3, 4, 5, 10 children, but as the distribution becomes more stable we'd split them into less children?

Number of key/index pairs to sample to determine new LinearModels: Right now I figure around 20 key/index pairs, but this is obviously changeable, with more key/index pairs leading to more accurate LinearModels but taking a longer time to construct. I was looking into the math behind Empirical CDF (ECDF) to figure out how many samples we'd need to properly approximate the CDF function.








Based on ALEX. The idea is to store (32-bit)key, <T>value pairs in Gapped Arrays 
that adaptively grow based on the distribution of the keys.
To determine which Gapped Array and which index in the Gapped Array to store the key,
we will build a "tree" of models with the GappedArrayNodes as the leaves. The interior nodes
will keep track of two child nodes each, and be responsible for prior linear transformations
of data (y_finalIndex = a3(a2(a1(X)+b1)+b2)+b3) 
These models will be linear models y=ax+b, determined by sampling a small subset
of the Gapped Array when the array is split into two (for now: This could potentially
be split into any number of children, lines,and arrays in the future, based on linear approximation
of the key distribution curve(We could even use polynomial approximation of the curve, but lets start with basics).)
The point of layering the linear model is two-fold. First, we want to minimize deviation
of the key distribution from uniform, as uniform is ideal and intuitively leads to the least
shifting by the GappedArray. Second, a high-level linear model lacks the precision to 
quickly locate the specific key or gap to insert, and we want to minimize time spent running exponential search.
Theoretically, the layered linear models can be compressed into a single piece-wise model (comprised of 
several line segments, the y of each line segment the index of the GA and the x of each line segment
the initial key), which is in effect what the layering is. This should reduce calculation time, but it is
unknown by how much(however, can be easily mathematically proven!)

Goal 1: Create a Gapped Array Node in C(++? maybe at first).
-Each Gapped Array contains 1 bitarray,
1 array for the Keys, and 1 array for the Values (Node<T>[] values; Basically an array of linked lists)
-Any movement in the keys will be tracked in the bitarray as well as simultaneously
occur for the Values array
-Contains a max density of the Gapped Array, exceeding which results in splitting the
array in two. I'm thinking about 0.5
-contains the final level of linear model, which will be a linear min-max transformation
in order to fit minimum and maximum of the keys in this particular Line Segment to the bounds of the
actual keys[].
-contains the prior level of the linear model, which will remain when the GANode transforms
into an interior node when it is split
-When in the process of transforming, will contain two "sample" arrays, consisting of uniform
sampling of all keys and indexes. This will be used to determine the new linear model, then will
be deleted along with the parent GA.

Goal 2: Create a LinearEqModel (class? struct?)
-really simple, just saves an lm.a, lm.b and takes an input x and spits out a y

Goal 3: Create a Min-MaxModel (class? struct?) 
-Used to fit the transformed keys to the structure of the array.

Goal 4: Find the cheapest implementation of Least-Squares Line (linear regression)
-Will be run on the two sample subsets of the GappedArray when it is split in two, in
order to determine the LinearEqModel of each of the subsequent Leaves

Goal 5: Find the most optimized version of exponential search for C(++).

Goal 5: Locate ranged data to test this structure. Might utilize latitude-longditude
dataset similar to ALEX paper, or might get some from the data dumps from Globus
