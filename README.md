# FRM Focus Rate Merge
Abstract:
The goal of this project is to combine a series of images of the same objects where each image has a different focus zone (or focused on a different object) to an image that is fully focused. The Algorithm creates a new image which is focused in most parts of the image while avoiding distorting the nature of the image that might be caused by focus strech and noisy pixels.

 Algorithm steps:
* Load n images
* Build a directed graph/network
* use min-cut partition to choose the best image for
each pixel

The graph architecture:
I have defined a network where of width*height*n vertices and 3*width*height*n edges, where height, width and n represent the size of the image and the number of the input images.
The edges connect between each pixel to his vertical and horizontal neighbor in the same image. The 3rd edge is connected to the pixel with same index at the next image (represented as a lower layer in the network).
Aditional SOURCE vertex is created with edges onnecting it to each pixel in the top image, and analogically a SINK vertex correspond to the bottom image.

Edge capacities:
There are 3 different types of capacities.
1. The edges linking the source to the first layer get a capacity of 100,000 which is much more than the rest of the capacities. This forces the network to choose one and only one pixel from each row, so each coordinate in the image gets only one vertex in the cut.
2. The edges between two layers get a constant capacity of (300/n)*0.5. They are the fine that the flow gets when it chooses to go to a lower layer.
This part is important to prevent the cut zig-zaging between distant images and so reduce ignoring noisy pixels.
3. The edges between each pixel to his vertical and horizontal neighbor get a capacity of g(x) where x is the Laplacian of the image. In the following, elaboration of what is g(x) and why the Laplacian.

Choosing g(x) and the meaning of laplacian:
The Laplacian represent to local “amount” of focus. It calculates how fast the gradient changes and takes in consideration the pixels in a distance of 2 for each direction.
The next step is to decide how to linearize the Laplacian using g(x).
So I printed the Laplacian and compared to the picture. I came to realization that the values of [0,17] should transfer to [200,300] and the values [70,255] to [0,100]. So I looked for a function that does that approximately and I found:

g(x) = 600/(1 + 𝑒^xb)
where b is given by input from the user and should meet the interval
of [0.1, 0.2].
Apparently, for high-resolution images when there are a lot more
details (not noise) for every pixel, b=2 will work much better because
then the capacity function (g) drops faster and allows to diffrentiate
between 1 unique pixel on a homogeneous neighborhood.

Conclusion:
The edge capacities ensure that you'll get one and one only pixel for each coordinate, and that the fine for taking a better Laplacian (better local focus) is balanced.

