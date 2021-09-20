#!/usr/bin/env python3
"""
Decodes/encodes *.lz files
"""
import argparse
import nlzss

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--decode", action="store_true")
parser.add_argument("-e", "--encode")
parser.add_argument("-i", "--input", required=True)
parser.add_argument("-o", "--output", required=True)

args = parser.parse_args()

if args.encode:
    nlzss.encode_file(args.input, args.output)

if args.decode:
    nlzss.decode_file(in_path=args.input, out_path=args.output)
