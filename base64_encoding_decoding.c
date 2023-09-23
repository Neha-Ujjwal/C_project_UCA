#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *encoderArr =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


int decoderArr[] = {
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 64, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, 52, 53, 54, 55, 56, 57,
      58, 59, 60, 61, -1, -1, -1, 64, -1, -1, -1, 0,  1,  2,  3,  4,  5,  6,
      7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
      25, -1, -1, -1, -1, -1, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
      37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
      -1, -1, -1, -1};

char *decode(char *encodedChar) {
  int i = 0;
  int encodedChar_len = strlen(encodedChar);
  char *decodedChar = (char *)malloc(3 * (encodedChar_len / 4) + 1);
 
  int n = 0;
  int decodingIndex = 0;
  while (i < encodedChar_len) {
    if(encodedChar[i]=='=')break;
    n = n << 6;
    n = n | (decoderArr[encodedChar[i]]);  //corresponding alphabetical value
    i++;
    if (i % 4 == 0) {
      decodedChar[decodingIndex++] = (char)((n >> 16) & 127);
      decodedChar[decodingIndex++] = (char)((n >> 8) & 127);
      decodedChar[decodingIndex++] = (char)((n)&127);
     
    }
  }

  //if there are characters remaining which are less than  4 so we have handle them separately
  if (i%4 == 3) {
    decodedChar[decodingIndex++] = (char)((n >> 10) & 127);
    decodedChar[decodingIndex++] = (char)((n >> 2) & 127);
  } else if (i%4 == 2) {
    decodedChar[decodingIndex++] = (char)((n >> 4) & 127);
  }
  decodedChar[decodingIndex] = '\0';
  return decodedChar;
}

char *encode(char *input) {

  int i = 0;
  int input_len = strlen(input);
  int single_double_char = input_len % 3;
  char *encodedString = (char *)malloc(4 * ((input_len + 2) / 3) + 1);
  int encodingIndex = 0;
  int n = 0;
  while (i < strlen(input)) {
    n = n << 8;
    n = n | input[i];
    i++;

    if (i % 3 == 0) {

      // first char
      encodedString[encodingIndex++] = encoderArr[(n >> 18) & 63];

      // second char
      encodedString[encodingIndex++] = encoderArr[(n >> 12) & 63];

      // third char
      encodedString[encodingIndex++] = encoderArr[(n >> 6) & 63];
//      printf("%d\n", (n >> 6) & 63);

      // fourth char
      encodedString[encodingIndex++] = encoderArr[(n)&63];
    }
  }

  //if there is less than 3 characters
  if (single_double_char == 1) {

    encodedString[encodingIndex++] = encoderArr[(n >> 2) & 63];
    encodedString[encodingIndex++] = encoderArr[(n << 4) & 63];
    encodedString[encodingIndex++] = '=';
    encodedString[encodingIndex++] = '=';

  } else if (single_double_char == 2) {
    encodedString[encodingIndex++] = encoderArr[(n >> 10) & 63];
    encodedString[encodingIndex++] = encoderArr[(n >> 4) & 63];
    encodedString[encodingIndex++] = encoderArr[(n << 2) & 63];
    encodedString[encodingIndex++] = '=';
  }
  encodedString[encodingIndex]='\0';
  return encodedString;
}



	

