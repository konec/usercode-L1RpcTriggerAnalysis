#!/usr/bin/perl -w

# Script to get the lists of files from single castor directory as an input to cfg.py
# Author: Katarzyna Romanowska-Rybinska
# Version 1.0 , Date: 17 July 2011
# Usage: ./getlist_one_dir.pl castor_directory filename

# define the input directory
$path = "/castor/cern.ch/user/r/rybinska";
# Check castor dir
if (!$ARGV[0])
{
        print ("Missing name of the castor directory!!\n");
        exit 1;
}
# Check filename
if (!$ARGV[1])
{
        print ("Missing name of the output file!!\n");
        exit 1;
}
$directory=$ARGV[0]; #get the first argument
$filename=$ARGV[1]; #get the second argument
chomp($directory); # delete the last character in the line which is \n
chomp($filename);# delete the last character in the line which is \n

# get the list of files
$filelist = `nsls $path/$directory`;
@files=split(/\n/,$filelist);
# create the file            
open(F,">$filename");
#begin loop over files
foreach (@files)
{
        $file=$_;
        chomp($file);# delete the last character in the line which is \n
        # write full filenames to the file
        print F "rfio:$path/$directory/$file\n";
} #end loop over files 
close(F);
