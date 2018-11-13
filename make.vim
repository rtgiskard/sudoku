" for .h .c .cxx .cpp
" sourced by ~/.vim/ftplugin/c.vim
" sourced by ~/.vim/ftplugin/cpp.vim

" user defined function
" allow calling without argument
function! <SID>Tex_todo_rt(...)

	let s:opts = ''
	if a:0 > 0
		for x in a:000
			let s:opts .= " ".x
		endfor
	endif

	" if write, same as 'make force', pay attention to the space
	" exec 'write!'
	exec ':!make' .s:opts
endfunction

" map method: /usr/share/vim/vimfiles/ftplugin/latex-suite/main.vim

" args: view + make_opts
command! -complete=customlist,Dort_opts -nargs=* Dxrt :call <SID>Tex_todo_rt(<f-args>)
command! Dx  :Dxrt
command! Dxr :Dxrt exec
command! Dxf :Dxrt format
command! Dxc :Dxrt clean
command! DxC :Dxrt clobber
command! Dxb :Dxrt backup

" completion
function! Dort_opts(ArgLead, CmdLine, CursorPos)
	return split("exec format force clean clobber backup"," ")
endfunction

" '.' is used as a delimeter, especialy in `let' expr, space is ignored
" let s:var, refer to: `help let'

" vim: set sts=4 ts=4 :
