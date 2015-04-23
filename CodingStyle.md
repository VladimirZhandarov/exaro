# Introduction #

This document describes the recommended coding style for eXaro.
Each line should end with a LF character ( **\n** ) (Unix termination). No CRLF (windows) or CR (Mac).
Each source should end with a empty line.

## Indentation ##
  * One tab
  * NO spaces, please update your editor settings !!!

## Variable declaration ##
  * Each variable declaration on a new line
  * Each new word in a variable name starts with a capital letter
  * Avoid abbreviations
  * Take useful names. No short names, except:
    * Single character variable names can denote counters and temporary variables whose purpose is obvious
    * Variables and functions start with a lowercase letter

Example:
```
// wrong
KProgressBar *prbar;
QString prtxt, errstr;

// correct
KProgressBar *downloadProgressBar;
QString progressText;
QString errorString;
```

## Whitespace ##
**Use blank lines to group statements** Use only one empty line
**Use one space after each keyword** For pointers or references, use a single space before '**' or '&', but not after** No space after a cast

Example:
```
// wrong
QString* myString;
if(true){
}

// correct
QString *myString;
if (true) 
{
}
```

## Braces ##
As a base rule, the left curly brace always have the opening brace on the start of a line.

Example:
```
// wrong
if (true) {
}

// correct
if (true)
{
}
```

Don't use curly braces when the body of a conditional statement contains only one line.

Example:
```
// wrong
if (true) {
    return true;
}

for (int i = 0; i < 10; ++i) {
    qDebug("%i", i);
}

// correct
if (true)
    return true;

for (int i = 0; i < 10; ++i)
    qDebug("%i", i);

```

## Switch statements ##
Case labels are on the same column as the switch

Example:
```
switch (myEnum) 
{
case Value1:
    doSomething();
    break;
case Value2:
    doSomethingElse();
    // fall through
default:
    defaultHandling();
    break;
}
```

## Line breaks ##
Try to keep lines shorter than 100 characters, inserting line breaks as necessary.