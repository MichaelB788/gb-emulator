#!/usr/bin/env python
import json
import os.path

# Script uses data from https://github.com/izik1/gbops/blob/master/dmgops.json
# Assumes dmgops.json is in the same directory as this script

file_dir = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(file_dir, "dmgops.json"), "r") as dmgops_json:
    data = json.load(dmgops_json)
    names_of = lambda prefix: ",\n".join(
        f'  "{entry["Name"]}"' for entry in data[prefix]
    )
    with open(os.path.join(file_dir, "mnemonics.h"), "w") as mnemonics_h:
        mnemonics_h.write(
            "\n".join(
                [
                    "#pragma once",
                    "",
                    "static const char *mnemonics[256] = {",
                    names_of("Unprefixed"),
                    "};",
                    "",
                    "static const char *mnemonics_cb[256] = {",
                    names_of("CBPrefixed"),
                    "};",
                ]
            )
        )
