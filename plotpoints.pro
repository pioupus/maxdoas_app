FUNCTION readmatrixfromfile,path
	OPENR, lun, path, /get_lun;
	n_rows=0
	rawrow = 'this is for showing idl thats a #$@/&§ string!!!!!'
	WHILE (NOT eof(lun)) DO BEGIN
		READF, lun, rawrow
		row = STRSPLIT(rawrow, /EXTRACT) 
		vec = float(row)
		n_columns = N_ELEMENTS(vec) ;
		IF n_rows eq 0 then $
			matrix_ =transpose(vec) ELSE $
			matrix_=[matrix_,transpose(vec)]      
		n_rows++
	ENDWHILE
	CLOSE,lun
	FREE_LUN,lun
	
	RETURN, matrix_
END

PRO plotpoints
	m = readmatrixfromfile('points.txt');
	print, N_ELEMENTS(m(*,0)), N_ELEMENTS(m(0,*))
	FOR i = 0,N_ELEMENTS(m(*,0))-1 DO BEGIN
		;print,m(i,0),m(i,1)
		xyouts, (m(i,0)),(m(i,1)), '0'
		;xyouts, 0.5,0.1, '0'
		;oplot, [m(i,0)],[m(i,1)], psym=4, symsize = 2, color = 255
	ENDFOR
END
