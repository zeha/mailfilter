#!/usr/bin/perl
# Perl script which parses a xml project file and writes a gnu-style makefile.
# modules used:
# http://search.cpan.org/author/GRANTM/XML-Simple-1.08/
# this module is already included with latest Perl:
# http://search.cpan.org/author/GSAR/Data-Dumper-2.101/
#
# 08-Okt-2002  (c) G.Knauf, info@gknw.de
#
# *** ATTENTION!! *** This is no ready-to-use script!
# It is only a quickhack to show how it could be done...
#
$VERSION = "0.01";
#

use Getopt::Std;
use XML::Simple;
use Data::Dumper;

$| = 1;
getopts('dos:t');
$xfile = $ARGV[0] || die "Usage: $0 <file to parse>\n";
die "Error: $xfile not found!\n" if (!-e $xfile);

$fname = (split(/\./,$xfile))[0];
$mfile = "$fname.makefile";
$outf = "$fname.dump";

print("Parsing file $xfile...\n");
# my $project = XMLin($xfile, parseropts => [ Namespaces => 1 ]);
my $project = XMLin("./$xfile");

if ($opt_o) {
  print("Printing to file $outf...\n");
  open(O, ">$outf") or die "Error: cant open file $outf for output!\n";
  print O Dumper($project);
  close(O);
}

print "----------------------------------------------------------------------\n";
$x = 0;
while ($tmp = $project->{TARGETORDER}->{ORDEREDTARGET}->[$x]->{NAME}) {
  printf ("Project %2d: %s\n",$x,$tmp);
  $x++;
}
print "----------------------------------------------------------------------\n";
$t = 0;
while ($project->{TARGETLIST}->{TARGET}->[$t]->{NAME}) {
  $x = 0;
  while ($tmp = $project->{TARGETLIST}->{TARGET}->[$t]->{LINKORDER}->{FILEREF}->[$x]->{PATH}) {
    printf ("%s\tFileref %2d: %s\n",$project->{TARGETLIST}->{TARGET}->[$t]->{NAME},$x,$tmp);
    $src[$t] .= " $tmp \\\n" if ($tmp =~ /\.c|\.def/);
    $obj[$t] .= " $tmp \\\n" if ($tmp =~ /\.o|\.lib/);
    $imp[$t] .= " $tmp \\\n" if ($tmp =~ /\.imp/);
    $xdc[$t] .= " $tmp \\\n" if ($tmp =~ /\.xdc/);
    $files[$t] .= " $tmp \\\n";
    $x++;
  }
  print "----------------------------------------------------------------------\n";
  $t++;
}

$s = 0;
while ($tmp = $project->{TARGETLIST}->{TARGET}->[0]->{SETTINGLIST}->{SETTING}->[$s]->{NAME}) {
  $upath = $s if ($tmp =~ /UserSearchPaths/);
  $spath = $s if ($tmp =~ /SystemSearchPaths/);
  print "Tmp: $tmp\n" if ($opt_d);
  last if ($upath && $spath);
  $s++;
}
print "----------------------------------------------------------------------\n" if ($opt_d);

for ($upath,$spath) {
  printf "$project->{TARGETLIST}->{TARGET}->[0]->{SETTINGLIST}->{SETTING}->[$_]->{NAME}:\n";
  $x = 0;
  while ($project->{TARGETLIST}->{TARGET}->[0]->{SETTINGLIST}->{SETTING}->[$_]->{SETTING}->[$x]->{SETTING}->[0]->{NAME} =~ /SearchPath/) {
    $tmp = $project->{TARGETLIST}->{TARGET}->[0]->{SETTINGLIST}->{SETTING}->[$_]->{SETTING}->[$x]->{SETTING}->[0]->{SETTING}->[0]->{VALUE};
    print "Tmp: $tmp\n";
    $x++;
    $inc .= " -I $tmp \\\n" if ($tmp !~ /HASH/);
  }
  print "----------------------------------------------------------------------\n";
}

#print "$inc\n";

write_makefile($mfile);

system("type $mfile");

exit;


#======================================================================

sub write_makefile {
  my ($file) = @_;
  print "Try to append to file $file...\n" if ($opt_d);
  open(OUT, ">$file") or die "Couldn't open $file: $!";
  print OUT <<EOM;
#
# *** Makefile cerated by $0 ***
# Source: $xfile
#

CC=mwccnlm
LINK=mwldnlm
CCFLAGS=

INCLUDE = \\
$inc

IMPOPT_T0 = \\
$imp[0]

IMPOPT_T1 = \\
$imp[1]

OBJS_T0 = \\
$obj[0]

OBJS_T1 = \\
$obj[1]

XDCDATA_T0 = \\
$xdc[0]

XDCDATA_T1 = \\
$xdc[1]

SOURCES_T0 = \\
$src[0]

SOURCES_T1 = \\
$src[1]

ALLFILES_T0 = \\
$files[0]

ALLFILES_T1 = \\
$files[1]

EOM
  close(OUT) or die "Couldn't close $file: $!";
}

sub append_txt {
  my ($file,$content) = @_;
  print "Try to append to file $file...\n" if ($opt_d);
  open(OUT, ">>$file") or die "Couldn't open $file: $!";
  print OUT $content;
  close(OUT) or die "Couldn't close $file: $!";
}

