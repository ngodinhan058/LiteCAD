with open('large.dxf', 'w') as f:
    f.write('  0\nSECTION\n  2\nENTITIES\n')
    for i in range(150000):
        f.write('  0\nLINE\n  8\n0\n 10\n%f\n 20\n%f\n 11\n%f\n 21\n%f\n' % (i, i, i+1, i+1))
    f.write('  0\nENDSEC\n  0\nEOF\n')
