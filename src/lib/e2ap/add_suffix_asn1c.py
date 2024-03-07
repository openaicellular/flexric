import os

# Change the path to the static library 
path_lib = "/home/mir/workspace/flr/build/src/lib/e2ap/v3_01/ie/asn/libe2ap_asn1_obj.a"
path = "/home/mir/workspace/flr/src/lib/e2ap/v3_01/"
suffix = "_e2ap_v3_01"

# Find functions in the static library
os.system("nm -C " + path_lib + " | grep -e r -e T -e d -e D > /tmp/tmp.txt")

# Load functions names present in the static library 
funcs = []
with open('/tmp/tmp.txt') as fp:
    for line in fp:
        p = line.split()
        # Find symbols in the initialized data section
        if len(p) > 2 and p[1] == "d":
            pos = p[2].find("asn")
            if pos != -1:
                print("Func d " + str(pos) + "   " + str(p[2]))
                funcs.append(p[2])

        # Find symbols in the initialized data section
        elif len(p) > 2 and p[1] == "D":
            pos = p[2].find("asn")
            if pos != -1:
                print("Func D " + str(pos) + "   " + str(p[2]))
                funcs.append(p[2])

        # Find symbols in the read only data section
        elif len(p) > 2 and p[1] == "r":
            pos = p[2].find("asn")
            if pos != -1:
                print("Func r " + str(pos) + "   " + str(p[2]))
                funcs.append(p[2])

        # Find symbols in the text (code) section (i.e., functions)
        elif len(p) > 2 and p[1] == "T":
            print("Func T " +  str(p[2]))
            funcs.append(p[2])

funcs.sort(key=str.lower)

# Replace function names with the new suffix name to avoid clashes while linking
for f in funcs:
    str1 = "find " + path + " -type f ! -name \"*txt\" | xargs sed -i -E \'/(include)/!s/" + f + "/" + f + suffix + "/g\'" 
    os.system(str1)

