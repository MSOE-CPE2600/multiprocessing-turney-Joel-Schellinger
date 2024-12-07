# System Programming Lab 11 Multiprocessing
## Implementation
To implement the features requested I used some of the methods provided in the mandel.c.
I used the getopt method to easily make commandline argument options to make the program userfriendly.
### Options

- -x \<coord\>&emsp;&emsp;&emsp;&emsp;X coordinate of the image center point. (default=-0.6702094071878258)

- -y \<coord\>&emsp;&emsp;&emsp;&emsp;Y coordinate of the image center point. (default=0.4580605576199168)

- -z \<zoom\>&emsp;&emsp;&emsp;&emsp;Zoom factor to scale down each image by (default=0.9)

- -p \<processes\>&emsp;&emsp;Set number of processes. (default=1)

- -i \<images\>&nbsp;&nbsp;&emsp;&emsp;&emsp;Set the number of images for the video. (default=50)

- -b&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Build and show the movie.

- -h&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;Show this help text.

### Multiprocessing
To implement the multiprocessing feature I used fork() to create multiple processes.
I forked the same amount of times as processes.
I then used the child process to compute the image.
The processes start processing images in chronological order.
e.g. process n processes image n.
When it is done it moves to image n + the number of processes specified.
The scale is calculated by taking the initial scale and multiplying it by the zoom_factor to the power of the image number.
I then use the provided methods to compute and save the image.

## Results
After implementation I made a plot of time vs. the number of processes used to make the images: 

![image](https://github.com/user-attachments/assets/ba27930b-e78e-424e-9247-2b2146770cdb)

The results show that as the number of processes doubles, the time required to process the images approximately halves, demonstrating the benefits of parallelism.
Since my computer has 20 cores, the optimal number of processes is 20. 
After this point, the program reaches a steady state where adding more processes does not improve performance.
This is because only 20 process can run in parallel at a time with 20 cores.

After adding multithreading we can compare the performance certain combinations of processes.
To demonstrate this I created a heat map of the times with processes on the y axis and threads on the x axis..
![image](https://github.com/user-attachments/assets/3b283648-0a48-48f5-8c6e-b7054d60bdee)



