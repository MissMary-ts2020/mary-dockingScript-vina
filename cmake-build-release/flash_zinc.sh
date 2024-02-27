#! /bin/bash
rm -rf zinc
mv zinc.bak zinc
cp -r zinc zinc.bak
rm -rf result_out
mkdir result_out
chmod -R 777 result_out
rm result.csv
touch result.csv
chmod -R 777 result.csv
