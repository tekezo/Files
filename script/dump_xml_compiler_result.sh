#!/bin/sh

system_xml_directory=/Library/org.pqrs/KeyRemap4MacBook/app/KeyRemap4MacBook.app/Contents/Resources
#private_xml_directory=/noexists
private_xml_directory=~/Library/Application\ Support/KeyRemap4MacBook

for command in \
    dump_data \
    dump_tree \
    dump_tree_all \
    dump_number \
    dump_identifier_except_essential \
    output_bridge_essential_config_index_hpp \
    ; do
    echo "------------------------------------------------------------"
    echo "$command old"
    /usr/bin/time /Library/org.pqrs/KeyRemap4MacBook/bin/dump_xml_compiler_result \
        $system_xml_directory "$private_xml_directory" $command > ~/$command.old
    echo "------------------------------------------------------------"

    echo "------------------------------------------------------------"
    echo "$command new"
    /usr/bin/time ../../KeyRemap4MacBook/src/bin/dump_xml_compiler_result/build/Release/dump_xml_compiler_result \
        $system_xml_directory "$private_xml_directory" $command > ~/$command.new
    echo "------------------------------------------------------------"

    echo "------------------------------------------------------------"
    echo "diff old new"
    diff -u ~/$command.old ~/$command.new
    echo "------------------------------------------------------------"
done
