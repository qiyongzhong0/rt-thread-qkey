from building import *

cwd = GetCurrentDir()
path = [cwd+'/inc']
src  = Glob('src/*.c')
 
group = DefineGroup('qkey', src, depend = ['PKG_USING_QKEY'], CPPPATH = path)

Return('group')