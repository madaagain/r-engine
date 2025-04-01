#!/bin/bash

while getopts 'gca:' OPTION; do
    case "$OPTION" in
        g)
            lcov --capture --directory ../ --output-file coverage.info
            genhtml coverage.info --output-directory coverage
            echo "Generated coverage infos."
            ;;
        c)
            rm -rf coverage
            rm coverage.info
            echo "Removed coverage infos."
            ;;
        ?) 
            echo "Script usage: ./generateCoverage.sh [-g] [-c]"
            echo "[-g]: Generate coverage infos."
            echo "[-c]: Remove coverage folders and files."
            exit 84
            ;;
    esac
done
