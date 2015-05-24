泛语言（Funny Language）
===================================

泛语言，又称范语言，是一种格式自由（free style）的程式语言。这里的“格式自由”，是相对的自由，而不是绝对自由。格式自由意味着可以自定义函数的语法格式，这种语法格式看起来就像我们说话一样自然。  
    
泛语言并不直接将代码转换为机器语言，而是将Lisp语言作为其中间语言。这么做的理由是“变化的语言”需要以一种“不变的语言”作为基础，这样才能以不变应万变。  
    
泛语言采用的是一种带参数的DFA技术。为什么是DFA，而不是NFA？那是因为DFA没有二义性，所以易于处理，而NFA需要转换为DFA，这种转换在带参数的情况下显得更加困难，完全没有必要。所以在定义语法模板的时候，必须原生就是DFA的。  
    
下面是泛语言在RHEL上运行的几个示例：  

	[root@192 funny]# ./funny
	Welcome to FUNNY programming world. Type {exit} to exit.
	> {define function {qsum}:
	      {the sum of squares {a} and {b}} as
	        {{{a}*{a}}+{{b}*{b}}}}
	qsum
	> {the sum of squares {2} and {3}}
	13
	> {define function {abs}: 
	      {the absolute value of {x}} as 
	        {for the conditions: 
	           when {{x}>{0}} then {x},
	           when {{x}={0}} then {0},
	           when {{x}<{0}} then {-{x}}}}
	abs
	> {the absolute value of {-2}}
	2
	> {the sum of squares {2} and {the absolute value of {-2}}}
	8
	> {the absolute value of {the sum of squares {2} and {-3}}}
	13
	> {define function {copy-files}:
	  {copy files {n} times from {src} to {dest}} as
	    {if {{n}>{0}} then {begin procedure 
	       {run command {copy file from {src} to {append string {dest} {convert atom {n} to string}}}} 
	       {copy files {{n}-{1}} times from {src} to {dest}}}}}
	copy-files
	> {copy files {4} times from {"/home/fgp/temp/abc"} to {"/home/fgp/temp/def"}}
	()
	> {exit}
	Goodbye!
	[root@192 funny]#

希望大家能喜欢上这门语言，并带动它发展！  

联系方式
===================================
    qq群：111086164  
    邮箱：fanguangping@163.com  

