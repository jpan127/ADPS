import json
import sys # Needed for nodejs call
data = {
    "degrees": 0,
    "confidence": 0,
    "vantage_detected": 1,
}

with open('read.json', 'w') as outfile:
	json.dump(data, outfile, sort_keys=True, indent=4)
    # sort_keys, indent are optional and used for pretty-writ