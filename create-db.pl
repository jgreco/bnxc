#!/usr/bin/perl

open(IN, "./files");

$album="";

while(<IN>) {
	@line = split(/\//);
	if($album eq @line[1]) {
		print "-";
		print @line[2];
	}
	else {
		$artist = @line[0];
		$album = @line[1];

		print "\n";
		print $artist;
		print "\n";
		print "1000 "; #I don't really care about the year, this could potentially be added later though
		print $album;
		print "\n-";
		print @line[2];
	}
}
