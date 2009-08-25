#!/usr/bin/perl

open(IN, "./tmp");

$album="";

while(<IN>) {
	@line = split(/\//);
	if($album eq @line[1]) {
		print @line[2];
		print $_;
	}
	else {
		$artist = @line[0];
		$album = @line[1];

		print "\n";
		print $artist;
		print "\n";
		print $album;
		print "\n";
		print @line[2];
		print $_;
	}
}
