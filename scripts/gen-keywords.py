#!/usr/bin/env python2

import os
import sys
from subprocess import check_output, CalledProcessError

KWSCRIPT = '''
import keyword, sys
sys.stdout.write(' '.join(keyword.kwlist))
'''

BUILTINSCRIPT = '''
import sys
sys.stdout.write(' '.join(dir(__builtins__)))
'''

def filter_kwds(kwds):
    stripkwds = ['print', 'exec']
    return [kwd for kwd in kwds if kwd not in stripkwds]

def get_kwlist(vers=[2,3]):
    kwds = ''
    for ver in vers:
        kwds += '%s ' % check_output(['python%d' % ver, '-c', KWSCRIPT])
    return filter_kwds(sorted(set(kwds.split()), key=lambda s: s.lower()))

def get_builtins(vers=[2,3]):
    kwds = ''
    for ver in vers:
        kwds += '%s ' % check_output(['python%d' % ver, '-c', BUILTINSCRIPT])
    return sorted(set(kwds.split()), key=lambda s: s.lower())

def backup_file(fn):
    bn = os.path.basename(fn)
    dn = os.path.dirname(fn)
    bfn = os.path.join(dn, '~%s' % bn)
    open(bfn, 'w').write(open(fn).read())

def update_ft_keywords(findkeys, do_backup=True):
    newtext = ''
    dn = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    fn = os.path.join(dn, 'data', 'filetypes.python')
    if do_backup:
        backup_file(fn)
    src_lines = open(fn).read().split('\n')
    new_text = ''
    for line in src_lines:
        for fk, kwds in findkeys.items():
            if line.startswith('%s=' % fk):
                new_text += '%s=%s\n' % (fk, kwds)
                break
        else:
            new_text += '%s\n' % line
    new_text = '%s\n' % new_text.rstrip()
    open(fn, 'w').write(new_text)

if __name__ == "__main__":
    py2n3kwds = get_kwlist([2,3])
    py2n3builtins = get_builtins([2,3])
    update_ft_keywords({'primary': ' '.join(py2n3kwds),
                        'identifiers': ' '.join(py2n3builtins)})
