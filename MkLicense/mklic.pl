#!/usr/bin/perl -w
#
###############

use POSIX qw(strftime);

sub getval {
	my $default = shift || "";
	my $val = <STDIN>;
	$val =~ s/\n//g;
	$val =~ s/\r//g;
	if ($val eq '') { $val = $default; }
	if ($val eq '') { print "  -> You must enter a value: "; $val = getval($default); }
	return $val;
}
print "MailFilter/ax License Generator 1.00\n";
print "(C) 2003 Christian Hofstaedtler\n";
print "\n";
print " Issuer [ch]: ";		$lic_issuer = getval("ch");
print " Customer: ";		$lic_cust = getval();
print " Contact [N/A]: ";	$lic_contact = getval("N/A");
print " Server: ";		$lic_server = getval();
print " Distributor [HIE]: ";	$lic_distr = getval("Hofstaedtler IE GmbH");
print " Dealer [N/A]: ";	$lic_dealer = getval("N/A");
#print " Product [MailFilter/ax Server]: ";	$lic_product = getval("MailFilter/ax Server");
print " Addons:\n";
print "  VScan [Y/N]: ";	$lic_add_vscan = getval();
print "  Alias [Y/N]: ";	$lic_add_alias = getval();
print "  MCopy [Y/N]: ";	$lic_add_mcopy = getval();
print "  BWMGT [Y/N]: ";	$lic_add_bwmgt = getval();
print " Platform [NetWare]: ";	$lic_plat = getval("NetWare");

print "-----------------------------------------------------\n\n";

$license = "MailFilter/ax R1 License File 1.00
(C) Copyright Christian Hofstaedtler

  Customer: $lic_cust
  Contact: $lic_contact
  Server: $lic_server
  Distributor: $lic_distr
  Dealer: $lic_dealer
  Product: MailFilter/ax Server
  Addons: ";
if (uc($lic_add_vscan) eq 'Y') { $license = $license . "VS/Integration "; }
if (uc($lic_add_alias) eq 'Y') { $license = $license . "Aliasing "; }
if (uc($lic_add_mcopy) eq 'Y') { $license = $license . "MailCopy "; }
if (uc($lic_add_bwmgt) eq 'Y') { $license = $license . "BandwidthControl "; }
$license = $license . "
  Platform: $lic_plat

  Initial Key 1.5 - ".strftime("%a %b %Y %H:%M",localtime())." GMT+1 - ".$lic_issuer."\n";

  $call = '"mklic '.$lic_plat.'" '." $lic_server $lic_add_vscan $lic_add_alias $lic_add_mcopy $lic_add_bwmgt";
  $mklic = `$call` || die "Could not start mklic executable";

$license = $license . $mklic;
print $license;

if ($ARGC > 0)
{
	open LICFILE,$ARGV[1];
	print LICFILE $license;
	close LICFILE;
	print "\nLicense written to ".$ARGV[1]."\n";
}


