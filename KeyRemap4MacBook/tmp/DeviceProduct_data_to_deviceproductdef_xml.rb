#!/usr/bin/ruby

print '<?xml version="1.0"?>', "\n"
print "<root>\n"

while l = gets
  next if l.strip.empty?
  next if /^\/\// =~ l.strip

  a = l.split

  print "  <deviceproductdef>\n"
  print "    <productname>#{a[0].strip}</productname>\n"
  print "    <productid>#{a[1].strip}</productid>\n"
  print "  </deviceproductdef>\n"
  print "\n"
end

print "</root>\n"
