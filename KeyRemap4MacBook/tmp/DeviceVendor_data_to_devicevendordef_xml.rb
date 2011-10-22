#!/usr/bin/ruby

print '<?xml version="1.0"?>', "\n"
print "<root>\n"

while l = gets
  next if l.strip.empty?
  next if /^\/\// =~ l.strip

  a = l.split

  print "  <devicevendordef>\n"
  print "    <vendorname>#{a[0].strip}</vendorname>\n"
  print "    <vendorid>#{a[1].strip}</vendorid>\n"
  print "  </devicevendordef>\n"
  print "\n"
end

print "</root>\n"
