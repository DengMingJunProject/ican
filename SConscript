# RT-Thread building script for trace component

from building import *

src_folder = 'device'

cwd = GetCurrentDir()

src = Glob('*.c')
CPPPATH = [ (cwd)]
if GetDepend(['GLP_ICAN_MASTER']):
	src+= Glob('icanM/*.c')
	CPPPATH  += [ os.path.join(cwd, 'icanM')]
	
if GetDepend(['GLP_ICAN_SLAVE']):
	src+= Glob('icanS/*.c')
	CPPPATH  += [ os.path.join(cwd, 'icanS')]


group = DefineGroup('ican', src, depend = ['GLP_USING_ICAN'], CPPPATH = CPPPATH)

Return('group')