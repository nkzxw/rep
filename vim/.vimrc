set nocompatible           
filetype off
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
"Plugin 'taglist-plus'
call vundle#end()           
filetype on
set mouse=
"set list
set number
set shiftwidth=4
set ff=unix "set fileformats=unix,dos
set cindent
set clipboard+=unnamed " Vim 的默认寄存器和系统剪贴板共享
"syntax on
set tags=tags;
"set autochdir "
set tags+=~/.vim/systags
let Tlist_Show_One_File = 1      "不同时显示多个文件的tag，只显示当前文件的 
let Tlist_Exit_OnlyWindow =  1   "如果taglist窗口是最后一个窗口，则退出vim 
let Tlist_Auto_Open = 1          "启动vim后自动打开taglist窗口
noremap <F8> :TlistToggle<CR> "设置taglist打开关闭的快捷键F8
"let g:AutoOpenWinManager = 1  "在进入vim时自动打开winmanager
"let g:winManagerWindowLayout = "TagList|FileExplorer,BufExplorer"
"let g:winManagerWindowLayout = 'FileExplorer|TagList' "你要是喜欢这种布局可以注释掉这一行
"noremap <F3> :WMToggle<cr> "映射F3为功能键调出winmanager的文件浏览器
