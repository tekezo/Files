#!/usr/bin/ruby

while l = gets
  break if /getApplicationType/ =~ l
end

equal = []
prefix = []
appname = nil

while l = gets
  next if l.strip.empty?
  next if 'if (! name) return 0;' == l.strip
  next if 'ConfigXMLParser* parser = [ConfigXMLParser getInstance];' == l.strip
  next if "{\n" == l
  next if /^\/\// =~ l.strip
  break if "}\n" == l

  if /return \[parser keycode:@"ApplicationType::(.+)"\]/ =~ l then
    appname = $1

  elsif /\[name isEqualToString:@"(.+)"\]/ =~ l then
    equal << $1

  elsif /\[name hasPrefix:@"(.+)"\]/ =~ l then
    prefix << $1

  elsif '}' == l.strip then
    if appname.nil? then
      throw :appname_is_nil
    end
    print "<appdef>\n"
    print "  <appname>#{appname}</appname>\n"
    equal.each do |e|
      print "  <equal>#{e}</equal>\n"
    end
    prefix.each do |p|
      print "  <prefix>#{p}</prefix>\n"
    end
    print "</appdef>\n\n"

    appname = nil
    equal = []
    prefix = []

  else
    p l
    throw :unknown_line
  end
end
