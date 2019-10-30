#!/usr/bin/env python3

from argparse import ArgumentParser, FileType
import sys, random


def parseErr(usage, msg, argParser):
    print(usage)
    argParser.exit(1, msg)


def main():
    usageMsg = "shuf.py [-h] [-i, --input-range=LO-HI] [-n, --head-count=COUNT] [-r] [FILE|-]"
    args = sys.argv[1:]

    parser = ArgumentParser(description="Print some random lines from a file.", usage=usageMsg)

    parser.add_argument('-i', '--input-range', type=str, action='store', nargs=1, dest='inputRange', help="""-i, --input-range=LO-HI;
    treat each number LO through HI as an input line""")

    parser.add_argument('-n', '--head-count', type=int, action='store', nargs=1, dest='count', help="""-n, --head-count=COUNT;
    output at most COUNT lines""")

    parser.add_argument('-r', '--repeat', action='store_true', dest='repeat', help="""-r, --repeat;
    output lines can be repeated""")

    parser.add_argument('randFile', nargs='?', type=FileType('r'), default=sys.stdin)

    parser.add_argument('-', action='store_true', dest='usesDash')

    pargs = parser.parse_args(args)

    # tells whether or not the program is trying to use an input file
    usesFile = (pargs.randFile != sys.stdin)

    # if a range is used, try and separate it into low and high values
    usesRange = (not pargs.inputRange is None)
    rangeLo = None
    rangeHi = None
    if usesRange:
        tempRange = pargs.inputRange[0].split('-')
        if len(tempRange) != 2:
            parseErr(usageMsg, "Invalid input range.", parser)
        try:
            rangeLo = int(tempRange[0])
            rangeHi = int(tempRange[1])
        except:
            parseErr(usageMsg, "Input range is not strictly integers.", parser)
        if rangeLo - rangeHi >= 2:
            parseErr(usageMsg, "Input range LO - HI must be greater than -1.", parser)

    # can only use either a dash or nothing specified, a file to read from, or an input range
    # BOOLEAN ALGEBRA
    if (pargs.usesDash and (usesFile or usesRange)) or (usesFile and usesRange):
        parseErr(usageMsg, "Too many arguments.", parser)

    # define boolean for using head-count, and throw error if head-count is less than 0
    usesHC = (not pargs.count is None)
    headCount = -1
    if usesHC:
        # we can guarantee headCount is valid if -n was used
        headCount = int(pargs.count[0])
        if headCount < 0:
            parseErr(usageMsg, "Invalid head-count argument.", parser)

    # define the lines to use for the actual printing
    lines = []
    if usesRange:
        for i in range(rangeLo, rangeHi + 1):
            lines += [i]
    else:
        lines = pargs.randFile.readlines()
        # strip newlines from list elements
        for i in range(0, len(lines)):
            lines[i] = lines[i].strip('\n')

    # now print the actual lines
    while len(lines) != 0:
        # if -n was used, make sure headCount hasn't run out
        if usesHC and headCount <= 0:
            break
        randLine = random.choice(lines)
        print(randLine)
        if not pargs.repeat:
            lines.remove(randLine)
        # decrement headCount, use it as an iterating variable
        if usesHC:
            headCount -= 1


if __name__ == "__main__":
    main()
