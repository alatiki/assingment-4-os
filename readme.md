## How to build
inside the directory where the code exist, type:
$ make

if you would like to clean the code and rebuild, then

$ make clean
$make

## How to run

To execute the code, you will need to build it  first then write the following command:

./cse4001_sync<problem #>

where the <problem #> represent the syncronization example.

For instance, to use the No-starve readers-writers solution, you have to call the code in this way:

./cse4001_sync 1

Available options are 1 to 4


## Code structure

The code is given in a single file where each problem is represented by its function.
I used C++ structures to collect the required data for each problem.

## Example Runs

Here are some examples of how to use the code as will as the expected output:

 ./cse4001_sync 1

![pic1](pr1.PNG)



./cse4001_sync 2
![pic1](pr2.PNG)


 ./cse4001_sync 3
![pic1](pr3.PNG)


./cse4001_sync 4
![pic1](pr4.PNG)









