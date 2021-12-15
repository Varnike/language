# Short function example

``` sh
Theorem (Taylor)	# function name                             
Given (x y)		# arguments                                 
Proof			# start of func                             
        Introduce(x);   # set x to input value

        Consider x = x + 1; assuming expression (x > 2) perfomed;                
                                                               
        suppose (y == 3) perfomed	# if construction
        therefore {			# if true                   
                x = y + 1;                                     
        } however {			# else                      
                y = x + 1;                                     
                proven x;		# return id                
        }            
	Conclusion(y);                  # print y value                            
        proven y * (x == 2);		# return expr              
QED			# end of func 
$
```
