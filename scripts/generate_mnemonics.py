#!/usr/bin/env python
import json

# Script uses data from https://github.com/izik1/gbops/blob/master/dmgops.json
# Assumes dmgops.json is in the same directory as this script

names_of = lambda prefix: ",\n".join(f'  "{entry["Name"]}"' for entry in data[prefix])

with open("dmgops.json", "r") as dmgops_json:
    data = json.load(dmgops_json)
    with open("mnemonics.h", "w") as output:
        output.write(
            "\n".join(
                [
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
