cALLiX - c Adaptive Linear Learned indeX

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
