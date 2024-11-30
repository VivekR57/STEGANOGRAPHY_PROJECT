#Steganography#

Description
This project implements LSB (Least Significant Bit) image steganography in C, allowing users to hide secret messages within image files, specifically .bmp files. The application enables users to embed a text message into the image without noticeable changes to the image itself. This technique leverages the LSB method, where the least significant bit of the pixel values is replaced with the bits of the secret message.

Abstract
Steganography is the art of concealing communication by embedding information within another medium, such as an image. Unlike cryptography, where the message is encrypted but still recognizable, steganography hides the fact that a message is even present. In this project, we focus on hiding a text message inside a .bmp image file. The process uses the Least Significant Bit (LSB) technique to embed the message, which is not visible to the human eye. The application also provides a mechanism to check if a message has been successfully embedded and to decrypt the hidden message.

The application is a command-line tool that allows users to:

Embed a text message into a .bmp image file.
Check if the image has enough space to accommodate the message.
Embed a "magic string" for identifying whether the image has been steganographically altered.
Decrypt the hidden message from the image.
Requirements
The application accepts a .bmp image file and a text file containing the message to be hidden.
The program analyzes the size of the text file and compares it with the image’s data capacity to ensure the message can fit.
It provides an option to embed a "magic string" (a predefined identifier) within the image to check if the image has been altered.
The application should allow users to decrypt the hidden message from the image.
The application is command-line based, where all inputs and options are passed via command-line arguments.
