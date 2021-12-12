# Short function example

``` sh
Theorem (Taylor)   # function name
Given (x, y)       # arguments
Proof              # start of func
	x = 3;
	
	Consider {
		x = x + 1;
	} assuming expression (x > 2) perfomed;

	suppose (y == 3) performed # if construction true/false
	therefore {                # if true
		x = y + 1;
	} however {                # else
		y = x + 1;
		proven x;           # return id
	}
	proven y * (x == 2);        # return expr
QED                # end of func	
```
