;Some examples of compiler code that should compile fine.
;I had added my reasoning for each one

:start	set [a], 5		;uses tabs
        set b, 5        ;uses spaces

		set a, 5		;case is not important
		set a, 5 

		set a, start	;labels should be fine
		set a,		end		;as should forward labels
		
:end	set a, 5
;:data	dat "test"		;should be able to handle data constructs
;		dat "test", 0	;and mixed data
;		dat 3, 5, 6		;of both types
		
;		set a,	5		;number of spaces should be no issue
