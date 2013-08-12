"Check to see if the filetype was automagically identified by Vim

if exists("did_load_filetypes")

    finish

else

    augroup filetypedetect

    au! BufRead,BufNewFile *.mm       setfiletype cpp
    au! BufRead,BufNewFile *.m       setfiletype c

    augroup END
endif
