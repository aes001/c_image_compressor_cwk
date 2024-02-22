[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/qunHzMJR)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=10896213)
# Assignment 2

- [Introduction](#introduction)
  * [Testing](#testing)
  * [The Algorithms](#the-algorithms)
    + [ebcBlock](#ebcblock)
      - [EBCC Images](#ebcc-images)
    + [ebcR32 and ebcR128](#ebcr32-and-ebcr128)
      - [The EBC Random Algorithm](#the-ebc-random-algorithm)
      - [EBCC Images](#ebcc-images-1)
- [This Repository](#this-repository)
- [Submission](#submission)

# Introduction

The hand-in date for this assignment will be the **10am on the 19th May**. Late submissions are permitted but with the standard University 5% per day penalty.

For assignment 2, the assignment is split into two parts as follows:

**1. 25 Marks - Improve Implementation of Assignment 1**


You should continue to work through the tasks in Assignment 1, using your existing repository and test script. You will receive some feedback on your code quality which should give you some idea of how to further improve this aspect of your code. Note that feedback will be provided within the University's standard 3 week time frame.

All the tasks and weighting remain the same as assignment 1, however this will be scaled down to 25 marks.

This must be submitted via GradeScope - Assignment 2 Part 1


**2. 25 Marks - Stretch Activity - Additional Compression Algorithms**

**This task relies on you having completed the code for ebc handling**.

Now that you have handled some simple compression (bit packing for ebc files), you will implement some more interesting and effective algorithms.

1. 20 Marks - ebcBlock - block-based compression using mean averages.
2. 15 Marks - ebcR32 - block-based compression using 32 random paradigm blocks.
3. 15 Marks - ebcR128 - block-based compression using 128 random paradigm blocks.

Each of these algorithms will be explained in their own sections below.

This must be submitted via GradeScope - Assignment 2 Part 2.


Again, 50% of the marks for the total assignment are for code quality - you can see the rubric which will be used to assess this on GradeScope.

# Testing

There will still be marks available for error checking, and you should ensure that your code is able to locate errors in the code. However, since your code will be working only with ebc files and data cannot be too high or low (as it is limited to 0-31 due to being written out in blocks of 5), these data tests can be ignored/removed.
For functionality, we will be looking at:
* Your code's performance on a variety of different sized images.
* How well you have implemented the algorithms
* The quality of your outputted images

For this assignment, you will not be provided with any test data as how you implement the algorithm may lead to minor differences between our final images - instead, you will be provided with a variety of final uncompressed images which have been converted to PGM image files which can be opened, which should give you some idea of what you should expect your own images to look like.

Your code quality will be assessed using the same rubric as in Assignment 1.

There are some tests set as visible in the Autograder which will allow you to check that your code has compiled correctly. If these are failing, please refer to the 'submission' section below for potential causes and fixes.

# The Algorithms

There are three algorithms for you to implement - two of them are quite similar and have been designed to test your ability to modularise and re-use your code.

For each algorithm, you should produce two executables - one to compress, and one to decompress the data.

The names of these executables should be:

| Algorithm      | Compression Executable | Decompression Executable |
|----------------|------------------------|--------------------------|
| ebcBlock       | ebcBlock               | ebcUnblock               |
| ebc random 32  | ebcR32                 | ebcU32                   |
| ebc random 128 | ebcR128                | ebcU128                  |

## ebcBlock

In this algorithm, your image must be split into 3x3 blocks. Each block should have a mean average value calculated from all existing pixels within the block.
Here is an example:

|   | col 0  | col 1  | col 2  |
|----|----|----|----|
| row 0 | 3  | 5  | 4 |
| row 1 | 7  | 5  | 11 |
| row 2 | 15 | 16 | 16 |

This 3x3 block has an average value of 82/9 = 9.11 - we will round this **down** to 9. You should use standard rounding - anything with a decimal part equal to or greater than 0.5 should be rounded up, anything less than 0.5 should be rounded down.

Therefore, this block will be represented by the single value 9 in our final image. When we decompress the file, all the pixels associated with the block will become 9.

Note that it is possible to have a block with fewer than 9 values - for example, on the edge of the image.

Once you have calculated these mean averages, you must write your data using packed binary - as with ebc images.

### EBCC Images

If you want to use file extensions to differentiate your different files, you can use .ebcc (ebc-compressed).

These compressed images will have the standard ebf-file-family header of:

```
magic number
height width (of the uncompressed image)
compressed image data
```

For files compressed using ebcBlock, the magic number should be **EC**.

## ebcR32 and ebcR128

As both of these algorithms use the same basic principle, they are explained together.

### The EBC Random Algorithm

This algorithm also requires you to create 3x3 blocks from your image.

In this family of algorithms, a set number of 3x3 'paradigm' blocks are chosen from the image. R32 takes 32 paradigm blocks, while R128 takes 128 paradigm blocks. These blocks will be used to represent every other block in the image and therefore it is important that you do not have any duplicate paradigm blocks. In cases where there are fewer than 32/128 blocks in the image, you can use fewer - it is up to you to decide how to best handle this in your code.

Each paradigm block should have an index assigned to it which should be between 0-31 for R32, and 0-127 for R128.

When you have chosen your paradigm blocks, you should iterate through the image and assign each 3x3 block the index (between 0-31, or 0-127) of its nearest paradigm block. You should decide on the closest block by the total difference between the blocks (again, how you do this is up to you, and you may want to try a few different things to see which gives you the best result in your final outputted image).

Your compressed file will contain both the paradigm blocks and your final image data, both written in bit-packed binary. ebcR32 uses 5 bits, but ebcR128 uses **7 bits** and therefore you will need to adapt your existing ebc-writing code to be able to handle this.

#### Randomness and Seeding

Because this algorithm relies on randomisation, it will have an additional argument - you must pass in the integer **seed** for the randomiser, and then use this to seed the randomiser in your code. You are not expected to develop any randomiser yourself, you can use the existing `rand()` function which is part of the standard C library. Seeding your code allows you to consistently reproduce errors or files, so will make testing significantly easier.

Therefore, an example of the use of this function is:
```
./ebcR32 my_file.ebc my_compressed_output.ebcc 8732
```

### EBCC Images

You can also use the .ebcc file extension for the outputs of ebcR32/ebcR128 - or use whatever extension you prefer to differentiate these files in your directory.

These compressed images will have the same header data as in all other ebf-family:

```
magic number
height width (of the uncompressed image)
paradigm blocks
compressed image data
```

#### Magic Numbers

| Algorithm | magic number |
|-----------|--------------|
| ebcR32    | E5           |
| ebcR128   | E7           |

# This Repository

For Part 1, you should continue to work in your Assignment 1 repository.

For Part 2, you may wish to take only the files you need from your Assignment 1 repository and work in this 'clean' repository, or you can continue working in Assignment 1 if you prefer - you will find that you will need some parts of your assignment 1 code.

In this repository, I am providing a few things which you may find useful:

 * Object files which convert ebf files to pgm files
 * Some example decompressed files which have been passed through the compression algorithms once.

The object files can be used to convert your ebf files into images which you can open using standard image manipulation software - this will be useful for visual debugging of your code. There are two objects - one compiled on Feng, one compiled on Codespace. I do not guarantee that these will work on other platforms (i.e. on Apple or Windows devices).

Note that the code includes almost no error checking- you should only be using it for valid ebf images.

The example images are provided to give you a sample of what an outputted image should look like- particularly in terms of how much visual noise you can expect.


# Submission

Two GradeScope assessments have been set up - one for your part 1 code, and one for part 2. Both allow multiple submissions.

You must submit:
* The .c and .h files required to create your executables.
* A working makefile which contains commands for 'clean' and 'all'.

To create a submission zip, you can use the following terminal command:
```
zip submission.zip *.c *.h makefile
```

This should create a 'submission.zip' which can be uploaded to GradeScope, alternatively you can use the drag and drop tool to select all of the files for your submission.

## 'My code won't compile on GradeScope'

The most common cause of this is having a subfolder in your submission. If this is the case, in your 'code' tab in GradeScope you will see 'folder_name/myfile.c' rather than just 'myfile.c'. You can fix this by following the instructions above and using the `zip` command.

Double check that your code compiles on Feng linux - if you have any compilation errors, these should be fixed prior to submission.

If you do not have any compilation errors, add the flag '-Wextra' to CFLAGS (or equivalent in your makefile) and recompile - this should highlight any additional compilation issues.

If this does not resolve your problem, please contact me via Teams (scsabr) during normal working hours (8am-6pm Mon-Fri) or speak to a lab demonstrator.
