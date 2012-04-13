#!/bin/sh

system_xml_directory=/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources
private_xml_directory=/noexists

for command in "dump_data" "dump_tree" "dump_number"; do
    echo "------------------------------------------------------------"
    echo "$command old"
    time /Library/org.pqrs/KeyRemap4MacBook/bin/dump_xml_compiler_result \
        $system_xml_directory $private_xml_directory $command > ~/$command.old
    echo "------------------------------------------------------------"

    echo "------------------------------------------------------------"
    echo "$command new"
    time ../../KeyRemap4MacBook/src/bin/dump_xml_compiler_result/build/Release/dump_xml_compiler_result \
        $system_xml_directory $private_xml_directory $command > ~/$command.new
    echo "------------------------------------------------------------"

    echo "------------------------------------------------------------"
    echo "diff old new"
    diff ~/$command.old ~/$command.new
    echo "------------------------------------------------------------"
done
