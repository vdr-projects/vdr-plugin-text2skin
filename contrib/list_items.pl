#!/usr/bin/perl

$ITEMFILE="common.h";
$PARMFILE="xml/object.h";

sub printo {
	my $text = shift;
	my $len = shift;
	print sprintf("%.*s", $len, $text);
	if (length($text) < $len) {
		print " " x ($len - length($text));
	}
}

sub printa {
	my $text = shift;
	my $len = shift;

	my @words = split(/\s+/, $text);
	my $offset = $len;
	while (@words) {
		$word = shift(@words);
		if ($offset + length($word) + 1 >= 80) {
			print "\n";
			print " " x $len;
			$offset = $len;
		}
		print "$word ";
		$offset += length($word) + 1;
	}
}

sub i18n {
	my $text = shift;
	if ($lang != 0) {
		return $TRANS{$text}->[$lang - 1];
	}
	return $text;
}

require 'contrib/items.doc';

$lang = "en";
if (defined($ARGV[0])) {
	$lang = $ARGV[0];
}
$lang = $LANGS{$lang};

print $INTRO[$lang];

$where = 0;
$until = "";

open DAT, "<$ITEMFILE" or die "Couldn't open $ITEMFILE: $!";
while (defined($_ = <DAT>)) {
	chomp;

	/^enum\s+eSkinSection/ and do {
		$where = 1;
		$until = "__SECTION_COUNT__";
		print i18n("Known Sections")."\n";
		print "-" x length(i18n("Known Sections"));
		print "\n\n";
		next;
	};

	/^enum\s+eSkinItem/ and do {
		$where = 2;
		$until = "__ITEM_COUNT__";
		print i18n("Known drawable Items")."\n";
		print "-" x length(i18n("Known drawable Items"));
		print "\n\n";
		next;
	};

	/^enum\s+eSkinDisplay/ and do {
		$where = 3;
		$until = "__DISPLAY_COUNT__";
		print i18n("Known Display-Items")."\n";
		print "-" x length(i18n("Known Display-Items"));
		print "\n\n";
		next;
	};

	(length($until) && /$until/) and do {
		$where = 0;
		$until = "";
		print "\n";
		next;
	};

	if ($where == 1) {
		next if /sectionSkin/;
		/section([\w]+)/ and do {
			$what = $1;
			printo i18n("Section:"), 15;
			print "[$what]\n";
			printo i18n("Description:"), 15;
			printa $SECTIONS{$what}->[$lang], 15;
			print "\n\n";
		};
	}

	if ($where == 2) {
		next if /itemUnknown/;
		next if /itemSkin/;
		/item([\w]+)/ and do {
			$what = $1;
			printo i18n("Item:"), 15;
			print "Item=$what\n";
			printo i18n("Description:"), 15;
			printa $ITEMS{$what}->[$lang], 15;
			print "\n";
			printo i18n("Parameters:"), 15;
			printa $ITEMPARAMS{$what}, 15;
			print "\n\n";
		};
	}

	if ($where == 3) {
		next if /displayAlways/;
		/display([\w]+)/ and do {
			$what = $1;
			printo i18n("Display:"), 15;
			print "display=$what\n";
			printo i18n("Description:"), 15;
			printa $DISPLAYS{$what}->[$lang], 15;
			print "\n";
			printo i18n("Parameters:"), 15;
			printa $DISPLAYPARAMS{$what}, 15;
			print "\n";
			printo i18n("Substitutes:"), 15;
			printa $DISPLAYSUBST{$what}, 15;
			print "\n\n";
		};
	}
}
close DAT;

open DAT, "<$PARMFILE" or die "Couldn't open $PARMFILE: $!";
while (defined($_ = <DAT>)) {
	chomp;

	/eSkinItem\s+mItem;/ and do {
		$where = 1;
		$until = "protected:";
		print i18n("Known Parameters")."\n";
		print "-" x length(i18n("Known Parameters"));
		print "\n\n";
		next;
	};

	(length($until) && /$until/) and do {
		$where = 0;
		$until = "";
		next;
	};

	if ($where == 1) {
		next if /mItem;\s*$/;
		/m([\w]+);\s*$/ and do {
			$what = lc($1);
			if ($what eq "pos1") {
				printo i18n("Parameter:"), 15;
				print "x1\n";
				printo i18n("Description:"), 15;
				print "\n";
				printo i18n("Default:"), 15;
				print "\n\n";
				printo i18n("Parameter:"), 15;
				print "y1\n";
				printo i18n("Description:"), 15;
				print "\n";
				printo i18n("Default:"), 15;
				print "\n\n";
			} elsif ($what eq "pos2") {
				printo i18n("Parameter:"), 15;
				print "x2\n";
				printo i18n("Description:"), 15;
				print "\n";
				printo i18n("Default:"), 15;
				print "\n\n";
				printo i18n("Parameter:"), 15;
				print "y2\n";
				printo i18n("Description:"), 15;
				print "\n";
				printo i18n("Default:"), 15;
				print "\n\n";
			} else {
				printo i18n("Parameter:"), 15;
				print "$what\n";
				printo i18n("Description:"), 15;
				print "\n";
				printo i18n("Default:"), 15;
				print "\n\n";
			}
		};
	}
}
close DAT;

