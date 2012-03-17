#!/usr/bin/ruby

$srcdir = '~/src/boost_1_49_0'
$basedir = File.dirname($0)

loop do
  iscontinue = false

  print '.'

  open("| LANG=C make 2>&1") do |f|
    while l = f.gets
      if /fatal error: '(boost\/.+)' file not found/ =~ l then
        file = $1
        destdir = "#{$basedir}/../include/#{File.dirname(file)}"
        system("mkdir -p #{destdir}")
        system("cp #{$srcdir}/#{file} #{destdir}")
        iscontinue = true
      end
    end
  end

  break unless iscontinue
end
