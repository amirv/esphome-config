def puku():
    return 123

def kuku(a=puku()):
    print(a)

kuku()
